/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* free_and_sbrk.c

   Test if free(3) actually lowers the program break.

   Usage: free_and_sbrk num-allocs block-size [step [min [max]]]

   Try: free_and_sbrk 1000 10240 2 1 1000
        free_and_sbrk 1000 10240 1 1 999
        free_and_sbrk 1000 10240 1 500 1000

        (Only the last of these should see the program break lowered.)
*/
/*#define _BSD_SOURCE*/
#define _DEFAULT_SOURCE
#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000

/******************************************************************************\
* My implementation of malloc and free.                                        *
*                                                                              *
* SYNOPSIS                                                                     *
*        void *malloc(size_t size);                                            *
*        void free(void *ptr);                                                 *
*                                                                              *
* DESCRIPTION                                                                  *
*        The malloc() function allocates size bytes and returns a pointer to   *
*        the allocated memory.  The memory is not initialized.  If size is 0,  *
*        then malloc() returns either NULL, or a unique pointer value that can *
*        later be successfully passed to free().                               *
*                                                                              *
*        The free() function frees the memory space pointed to by ptr, which   *
*        must have been returned by a previous call to malloc().  Otherwise,   *
*        or if free(ptr) has already been called before, undefined behavior    *
*        occurs.  If ptr is NULL, no operation is performed.                   *
*                                                                              *
* RETURN VALUE                                                                 *
*        The malloc() functions return a pointer to the allocated memory,      *
*        which is suitably aligned for any built-in type.  On error, these     *
*        functions return NULL.  NULL may also be returned by a successful     *
*        call to malloc() with a size of zero.                                 *
*                                                                              *
*        The free() function returns no value.                                 *
*                                                                              *
* ERRORS                                                                       *
*        malloc() can fail with the following error:                           *
*                                                                              *
*        ENOMEM Out  of  memory.  Possibly, the application hit the RLIMIT_AS  *
*        or RLIMIT_DATA limit described in getrlimit(2).                       *
\******************************************************************************/

static const intptr_t chunk_size    = 0x23000;
typedef struct {
    size_t allocated;
    void *prev_free_area;
    void *next_free_area;
    unsigned int padding; /* to 32 bytes */
    } free_list_entry_t;
static free_list_entry_t *free_list = NULL;
static const size_t min_allocation = sizeof(free_list_entry_t) - sizeof(size_t);

void *my_malloc(size_t size)
{
    errno = 0;
    if (size <= 0) return NULL;
    
    /* -----------------------------------------sizeof(free_list_entry_t) - sizeof(size_t)--------------------------------
     * Implementation restriction - all allocation requests are to fit inside a
     * single chunk.
     * ---------------------------------------------------------------------- */ 
    if (size + sizeof(size_t) > chunk_size)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* -------------------------------------------------------------------------
     * Ensure that the requested size is a multiple of the minimal allocation
     * which is the size of the free list header plus padding.
     * ---------------------------------------------------------------------- */ 
    if (size < min_allocation)
    {
        size = min_allocation;
    } else {
        unsigned int mask = sizeof(free_list_entry_t) - 1;
        if ((size + sizeof(size_t)) & mask != 0)
        {
            size += min_allocation - ((size + sizeof(size_t)) & mask);
        }
    } 
    
    /* -------------------------------------------------------------------------
     * If there is no free list yet, create a free area of one chunk only.
     * ---------------------------------------------------------------------- */   
    if (free_list == NULL) {
        free_list = (free_list_entry_t *)sbrk(chunk_size);
        if ((void *)free_list == (void *)-1) return NULL;
        free_list -> allocated      = chunk_size - sizeof(size_t);
        free_list -> prev_free_area = NULL;
        free_list -> next_free_area = NULL;
    }
    
    /* -------------------------------------------------------------------------
     * First fit allocation. (This is the simplest to implement.)
     * ---------------------------------------------------------------------- */ 
    free_list_entry_t *candidate_area = NULL;
    free_list_entry_t *last_free_area = NULL;
    for (candidate_area = free_list;
        candidate_area != NULL && candidate_area -> allocated < size;
        candidate_area  = candidate_area -> next_free_area)
    {
        last_free_area = candidate_area;
    }

    /* -------------------------------------------------------------------------
     * If there is insufficient free space available, do NOT request any new
     * allocation. Just return an error.
     * ---------------------------------------------------------------------- */ 
    if (candidate_area == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* -------------------------------------------------------------------------
     * Split the free area into used and free.
     * If the free area matches exactly the required list, take the entire
     * free list entry out of the doubly linked-linked list.
     * Otherwise, take away enough space from the lower part for the allocation
     * reuquest, and put the remainder back into the doubly-linked list.
     * ---------------------------------------------------------------------- */
    if (candidate_area -> allocated == size)
    {
        if (last_free_area != NULL)
        {
            last_free_area -> next_free_area = candidate_area -> next_free_area;
            if (candidate_area -> next_free_area != NULL)
            {
                ((free_list_entry_t *)(candidate_area -> next_free_area)) -> prev_free_area = last_free_area;
            }
        }
        else
        {
            free_list = candidate_area -> next_free_area;
        }
    }
    else
    {
        free_list_entry_t *split_free_area = (free_list_entry_t *)((char *)candidate_area + size + sizeof(size_t));
        split_free_area  -> allocated      = candidate_area -> allocated - size - sizeof(size_t);
        candidate_area   -> allocated      = size;
        split_free_area  -> next_free_area = candidate_area -> next_free_area;
        if (last_free_area == NULL)
        {
            free_list                        = candidate_area -> next_free_area;
        }
        else
        {
            last_free_area -> next_free_area = split_free_area;
        }
    }
    
    void *allocated_area        = (void *)((char *)candidate_area + sizeof(size));
    return allocated_area;
}

/******************************************************************************\
* Just add the returned area to the front of the doubly-linked list.           *
* This will lead to memory fragmentation.                                      *
\******************************************************************************/

void my_free(void *ptr)
{
    free_list_entry_t *new_free_area = (free_list_entry_t *)((char *)ptr - sizeof(size_t));
    new_free_area -> next_free_area  = free_list;
    new_free_area -> prev_free_area  = NULL;
    if (free_list -> next_free_area != NULL)
    {
        ((free_list_entry_t *)(free_list -> next_free_area)) -> prev_free_area = new_free_area;
    }
    free_list = new_free_area;
}

/******************************************************************************\
* Test harnass                                                                 *
\******************************************************************************/


int
main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

    printf("\n");

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);

    numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
    if (numAllocs > MAX_ALLOCS)
        cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);

    blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    freeMin =  (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    freeMax =  (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

    if (freeMax > numAllocs)
        cmdLineErr("free-max > num-allocs\n");

    void *old_prog_brk = sbrk(0);
    printf("Initial program break:          %10p\n", old_prog_brk);

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) {
        ptr[j] = my_malloc(blockSize);
        if (ptr[j] == NULL)
            errExit("mY_malloc");
        void *curr_prog_brk = sbrk(0);
        if (curr_prog_brk != old_prog_brk)
        {
            printf("Current prog break after #%4d: %10p (Increase of 0X%010lX)\n", j, curr_prog_brk, curr_prog_brk - old_prog_brk);
            old_prog_brk = curr_prog_brk;
        }
    }

    printf("Program break is now:           %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",
                freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep)
        my_free(ptr[j]);

    printf("After free(), program break is: %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}
