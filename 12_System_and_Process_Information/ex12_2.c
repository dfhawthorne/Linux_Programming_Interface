/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
*                                                                         *
* This code is based upon list_files.c.                                   *
\*************************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/******************************************************************************\
* Local constants and types                                                    *
\******************************************************************************/

#define PROCESS_NAME_MAX_SIZE 32
struct node
{
    pid_t pid;
    char process_name[PROCESS_NAME_MAX_SIZE+1];
    struct node *sibling;
    struct node *children;
};
struct orphan
{
    struct node *child;
    struct orphan *prev;
    struct orphan *next;
};

/******************************************************************************\
* Add a new node                                                               *
\******************************************************************************/

struct node *new_node(pid_t pid, char *process_name)
{
    struct node *result = NULL;
    
    result = (struct node *)malloc(sizeof(struct node));
    if (result == NULL)
    {
        fprintf(stderr, "malloc for node: %m\n");
        return NULL;
    }
    result -> pid = pid;
    strncpy(result -> process_name, process_name, PROCESS_NAME_MAX_SIZE);
    result -> process_name[PROCESS_NAME_MAX_SIZE] = '\0';
    result -> sibling  = NULL;
    result -> children = NULL;
    return result;
}

/******************************************************************************\
* Find a node with the given process id within a tree                          *
* This is a depth-first search (children before siblings).                     *
\******************************************************************************/

struct node *find_node(pid_t pid, struct node *root)
{
    if (root == NULL)
    {
        #ifdef DEBUG
        fprintf(stderr, "find_node(%ld, NULL)\n", (long)pid);
        fprintf(stderr, "find_node returned NULL\n");
        #endif
        return NULL;
    }

    #ifdef DEBUG
    fprintf(stderr, "find_node(%ld, %ld)\n", (long)pid, (long)(root->pid));
    #endif
    
    if (root -> pid == pid)
    {
        #ifdef DEBUG
        fprintf(stderr, "find_node returned ROOT\n");
        #endif
        return root;
    }
    
    struct node *result = NULL;
    
    result = find_node(pid, root -> children);
    if (result != NULL)
    {
        #ifdef DEBUG
        fprintf(stderr, "find_node returned CHILD\n");
        #endif
        return result;
    }

    result = find_node(pid, root -> sibling);

    #ifdef DEBUG
    fprintf(
        stderr,
        "find_node returned %s\n",
        (result == NULL) ? "NULL" : "SIBLING"
        );
    #endif
    return result;   
}

/******************************************************************************\
* List all active processes as a tree                                          *
\******************************************************************************/

static void print_tree(struct node *root, int level)
{
    if (root == NULL) return;
    
    for (int i = 0; i < level; i++)
    {
        putchar(' ');
        putchar(' ');
    }
    puts(root -> process_name);
    print_tree(root -> children, level+1);
    print_tree(root -> sibling,  level);
}

/******************************************************************************\
* List all active processes as a tree                                          *
\******************************************************************************/

static void             /* List all processes */
listProcs()
{
    DIR *dirp;
    char status_fn[269];
    char buffer[1024];
    struct node *root = NULL;
    struct orphan *orphans = NULL;
    
    root = new_node(0, "ROOT");
    if (root == NULL)
    {
        fprintf(stderr, "Unable to allocate root node: %m\n");
        return;
    }
    dirp = opendir("/proc");
    if (dirp  == NULL)
    {
        fprintf(stderr, "opendir failed on '/proc': %m\n");
        return;
    }

    /* ---------------------------------------------------------------------
       For each entry in the /proc directory, find a process sub-directory
       which is identified as starting with a digit. In that sub-directory,
       we want the 'status' file which has the process name, and the process
       ID of its parent.
       --------------------------------------------------------------------- */

    for (;;) {
        struct dirent *dp;
        
        errno = 0;              /* To distinguish error from end-of-directory */
        if ((dp = readdir(dirp)) == NULL)
            break;

        if (isdigit(dp -> d_name[0]) == 0)
            continue;           /* Skip /proc entries that are not processes */
        #ifdef DEBUG
        fprintf(stderr, "PID=%s\n", dp->d_name);
        #endif
        sprintf(status_fn, "/proc/%s/status", dp->d_name);
        #ifdef DEBUG
        fprintf(stderr, "File_name='%s'\n", status_fn);
        #endif
        /* ------------------------------------------------------------------
           Get the parent process ID, and the name of this process
           ------------------------------------------------------------------ */
        FILE *fp = fopen(status_fn, "r");
        if (fp != NULL)
        {
            char process_name[PROCESS_NAME_MAX_SIZE];
            pid_t process_pid, process_ppid;
            struct node *current_node = NULL;
            struct node *parent_node  = NULL;        
            
            process_pid = (pid_t)atoi(dp -> d_name);
            
            while (!feof(fp))
            {
                fgets(buffer, 1024, fp);
                if (strncmp(buffer, "Name:\t", 6) == 0)
                {
                    strncpy(process_name, buffer+6, PROCESS_NAME_MAX_SIZE);
                    process_name[PROCESS_NAME_MAX_SIZE] = '\0';
                    int len = strlen(process_name);
                    if (process_name[len-1] == '\n')
                    {
                        process_name[len-1] = '\0';
                    }
                }
                if (strncmp(buffer, "PPid:\t", 6) == 0)
                {
                    sscanf(buffer, "PPid:\t%d", &process_ppid);
                    break;
                }
            }
            fclose(fp);
            
            /* --------------------------------------------------------------
               Is this process the missing parent of an orphan?
               If so, claim the orphan, and remove the orphan from the
               orphanage.
               -------------------------------------------------------------- */
            
            current_node = NULL;
            for (struct orphan *foundling = orphans;
                foundling != NULL;
                foundling = foundling -> next)
            {
                if (foundling -> child -> pid == process_pid)
                {
                    current_node = foundling -> child;
                    strncpy(
                        current_node -> process_name,
                        process_name,
                        PROCESS_NAME_MAX_SIZE
                        );
                    current_node -> process_name[PROCESS_NAME_MAX_SIZE] = '\0';
                    /* This foundling is the last on the list of orphans */
                    if (foundling -> next != NULL)
                    {
                        foundling -> next -> prev = foundling -> prev;
                    }
                    /* This foundling is the first on the list of orphans */
                    if (foundling == orphans)
                    {
                        orphans         = foundling -> next;
                        orphans -> prev = NULL;
                    }
                    else
                    {
                        foundling -> prev -> next = foundling -> next;
                    }
                    free(foundling);
                    break;
                }
            }
    
            if (current_node == NULL) /* Not a found parent */
            {
                current_node = new_node(process_pid, process_name);
            }
            parent_node  = find_node(process_ppid, root);
            if (parent_node == NULL) /* A new orphan */
            {
                /* search orphans */
                struct orphan *curr_orphan = NULL;
                parent_node = NULL;
                for (curr_orphan = orphans;
                    curr_orphan != NULL && parent_node == NULL;
                    curr_orphan = curr_orphan -> next)
                {
                    parent_node = find_node(process_ppid, curr_orphan -> child);
                }
            }
            if (parent_node != NULL)
            {
                if (parent_node -> children == NULL)
                {
                    parent_node -> children = current_node;
                }
                else
                {
                    struct node
                        *ptr       = NULL,
                        *last_node = NULL;
                        
                    for (ptr = parent_node -> children;
                        ptr -> sibling != NULL;
                        ptr = ptr -> sibling)
                    {
                        if (strcmp(ptr -> process_name, current_node -> process_name) >= 0)
                        {
                            break;
                        }
                        last_node = ptr;
                    }
                    
                    if (last_node == NULL)
                    {
                        struct node *first_child = parent_node -> children;
                        parent_node -> children  = current_node;
                        current_node -> sibling  = first_child;
                    }
                    else
                    {
                        last_node -> sibling     = current_node;
                        current_node -> sibling  = ptr;
                    }
                }
            }
            else
            {
                parent_node = malloc(sizeof(struct node));
                if (parent_node == NULL)
                {
                    fprintf(
                        stderr,
                        "Unable to allocate parent for orphan: %m\n"
                        );
                    exit(1);
                }
                parent_node -> pid      = process_ppid;
                strcpy(parent_node -> process_name, "** Unknown **");
                parent_node -> sibling  = NULL;
                parent_node -> children = current_node;
                struct orphan *new_orphan = malloc(sizeof(struct orphan));
                if (new_orphan == NULL)
                {
                    fprintf(stderr, "Unable to allocate orphan: %m\n");
                    exit(1);
                }
                new_orphan -> child = parent_node;
                new_orphan -> prev  = NULL;
                new_orphan -> next  = orphans;
                if (orphans != NULL)
                {
                    orphans    -> prev  = new_orphan;
                }
                orphans             = new_orphan;
            }
        }
    }

    if (errno != 0) {
        fprintf(stderr, "readdir: %m\n");
    }

    if (closedir(dirp) == -1)
        fprintf(stderr, "closedir: %m\n");
        
    /* ---------------------------------------------------------------------- *
     * Add orphans as children to the root process (0)                        *
     * ---------------------------------------------------------------------- */
    
    if (orphans != NULL)
    {    
        struct node *last_child;
        for (last_child = root -> children;
            last_child != NULL && last_child -> sibling != NULL;
            last_child = last_child -> sibling
            )
        {
        }
        if (last_child == NULL)
        {
            root -> children = orphans -> child;
            struct orphan *p = orphans;
            orphans = orphans -> next;
            free(p);
            last_child = root -> children;
        }
        for (struct orphan *p = orphans;
            p != NULL;
            )
        {
            last_child -> sibling = p -> child;
            last_child            = p -> child;
            struct orphan *q      = p;
            p                     = p -> next;
            free(q);
        }
    }
    
    print_tree(root -> children, 0);
}

int
main(int argc, char *argv[])
{
    listProcs();

    exit(0);
}
