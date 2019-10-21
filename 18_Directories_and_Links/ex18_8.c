/******************************************************************************\
* Exercise 18-8: Implement ntfw().                                             *
*                                                                              *
* DESCRIPTION                                                                  *
*        nftw()  walks  through the directory tree that is located under the   *
*        directory dirpath, and calls fn() once for each entry in the  tree.   *
*        By  default, directories are handled before the files and subdirec‐   *
*        tories they contain (preorder traversal).                             *
*                                                                              *
*        To avoid using up all of the calling  process's  file  descriptors,   *
*        nopenfd  specifies  the  maximum  number of directories that nftw()   *
*        will hold open simultaneously.  When the search depth exceeds this,   *
*        nftw() will become slower because directories have to be closed and   *
*        reopened.  nftw() uses at most one file descriptor for  each  level   *
*        in the directory tree.                                                *
*                                                                              *
*        For each entry found in the tree, nftw() calls fn() with four argu‐   *
*        ments: fpath, sb, typeflag, and ftwbuf.  fpath is the  pathname  of   *
*        the  entry,  and  is expressed either as a pathname relative to the   *
*        calling process's current working directory at the time of the call   *
*        to  nftw(),  if dirpath was expressed as a relative pathname, or as   *
*        an absolute pathname, if dirpath was expressed as an absolute path‐   *
*        name.   sb is a pointer to the stat structure returned by a call to   *
*        stat(2) for fpath.                                                    *
*                                                                              *
*        The typeflag argument passed to fn() is an integer that has one  of   *
*        the following values:                                                 *
*                                                                              *
*        FTW_F  fpath is a regular file.                                       *
*                                                                              *
*        FTW_D  fpath is a directory.                                          *
*                                                                              *
*        FTW_DNR                                                               *
*               fpath is a directory which can't be read.                      *
*                                                                              *
*        FTW_DP fpath  is a directory, and FTW_DEPTH was specified in flags.   *
*               (If FTW_DEPTH was not specified in flags,  then  directories   *
*               will  always be visited with typeflag set to FTW_D.)  All of   *
*               the files and subdirectories within  fpath  have  been  pro‐   *
*               cessed.                                                        *
*                                                                              *
*        FTW_NS The  stat(2)  call  failed on fpath, which is not a symbolic   *
*               link.  The probable cause for this is that  the  caller  had   *
*               read  permission  on the parent directory, so that the file‐   *
*               name fpath could be seen, but did not have  execute  permis‐   *
*               sion,  so  that  the  file could not be reached for stat(2).   *
*               The contents of the buffer pointed to by sb are undefined.     *
*                                                                              *
*        FTW_SL fpath is a symbolic link, and FTW_PHYS was set in flags.       *
*                                                                              *
*        FTW_SLN                                                               *
*               fpath is a symbolic link pointing  to  a  nonexistent  file.   *
*               (This  occurs  only if FTW_PHYS is not set.)  On most imple‐   *
*               mentations, in this case the sb argument passed to fn() con‐   *
*               tains  information  returned  by  performing lstat(2) on the   *
*               symbolic link.  For the details on Linux, see BUGS.            *
*                                                                              *
*        The fourth argument (ftwbuf) that nftw() supplies when calling fn()   *
*        is a pointer to a structure of type FTW:                              *
*                                                                              *
*            struct FTW {                                                      *
*                int base;                                                     *
*                int level;                                                    *
*            };                                                                *
*                                                                              *
*        base  is  the  offset of the filename (i.e., basename component) in   *
*        the pathname given in fpath.  level is the depth of  fpath  in  the   *
*        directory  tree,  relative  to the root of the tree (dirpath, which   *
*        has depth 0).                                                         *
*                                                                              *
*        To stop the tree walk, fn() returns a  nonzero  value;  this  value   *
*        will become the return value of nftw().  As long as fn() returns 0,   *
*        nftw() will continue either until it has traversed the entire tree,   *
*        in  which case it will return zero, or until it encounters an error   *
*        (such as a malloc(3) failure), in which case it will return -1.       *
*                                                                              *
*        Because nftw() uses dynamic data structures, the only safe  way  to   *
*        exit out of a tree walk is to return a nonzero value from fn().  To   *
*        allow a signal to terminate the walk without causing a memory leak,   *
*        have  the handler set a global flag that is checked by fn().  Don't   *
*        use longjmp(3) unless the program is going to terminate.              *
*                                                                              *
*        The flags argument of nftw() is formed by ORing zero or more of the   *
*        following flags:                                                      *
*                                                                              *
*        FTW_ACTIONRETVAL (since glibc 2.3.3)                                  *
*               If  this glibc-specific flag is set, then nftw() handles the   *
*               return value from fn() differently.  fn() should return  one   *
*               of the following values:                                       *
*                                                                              *
*               FTW_CONTINUE                                                   *
*                      Instructs nftw() to continue normally.                  *
*                                                                              *
*               FTW_SKIP_SIBLINGS                                              *
*                      If fn() returns this value, then siblings of the cur‐   *
*                      rent entry will be skipped, and processing  continues   *
*                      in the parent.                                          *
*                                                                              *
*               FTW_SKIP_SUBTREE                                               *
*                      If  fn()  is called with an entry that is a directory   *
*                      (typeflag is FTW_D), this return value  will  prevent   *
*                      objects  within  that  directory from being passed as   *
*                      arguments to fn().  nftw() continues processing  with   *
*                      the next sibling of the directory.                      *
*                                                                              *
*               FTW_STOP                                                       *
*                      Causes  nftw()  to return immediately with the return   *
*                      value FTW_STOP.                                         *
*                                                                              *
*               Other return values could be associated with new actions  in   *
*               the  future;  fn() should not return values other than those   *
*               listed above.                                                  *
*                                                                              *
*               The feature test macro _GNU_SOURCE must be  defined  (before   *
*               including  any  header files) in order to obtain the defini‐   *
*               tion of FTW_ACTIONRETVAL from <ftw.h>.                         *
*                                                                              *
*        FTW_CHDIR                                                             *
*               If set, do a chdir(2) to each directory before handling  its   *
*               contents.   This  is  useful if the program needs to perform   *
*               some action in the directory in which fpath resides.  (Spec‐   *
*               ifying  this  flag  has  no  effect  on the pathname that is   *
*               passed in the fpath argument of fn.)                           *
*                                                                              *
*        FTW_DEPTH                                                             *
*               If set, do a post-order traversal, that is,  call  fn()  for   *
*               the  directory  itself  after  handling  the contents of the   *
*               directory and its subdirectories.  (By default, each  direc‐   *
*               tory is handled before its contents.)                          *
*                                                                              *
*        FTW_MOUNT                                                             *
*               If  set, stay within the same filesystem (i.e., do not cross   *
*               mount points).                                                 *
*                                                                              *
*        FTW_PHYS                                                              *
*               If set, do not follow symbolic links.   (This  is  what  you   *
*               want.)  If not set, symbolic links are followed, but no file   *
*               is reported twice.                                             *
*                                                                              *
*               If FTW_PHYS is not set, but FTW_DEPTH is set, then the func‐   *
*               tion  fn()  is  never called for a directory that would be a   *
*               descendant of itself.                                          *
*                                                                              *
* RETURN VALUE                                                                 *
*        These functions return 0 on success, and -1 if an error occurs.       *
*                                                                              *
*        If  fn()  returns nonzero, then the tree walk is terminated and the   *
*        value returned by fn() is  returned  as  the  result  of  ftw()  or   *
*        nftw().                                                               *
*                                                                              *
*        If  nftw()  is called with the FTW_ACTIONRETVAL flag, then the only   *
*        nonzero value that should be used by fn()  to  terminate  the  tree   *
*        walk  is  FTW_STOP,  and  that  value  is returned as the result of   *
*        nftw().                                                               *
*                                                                              *
\******************************************************************************/

#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/******************************************************************************\
* Global data area used for test NFTW procedure                                *
\******************************************************************************/

static struct
{
    int numRegularFiles,            /* FTW_F && S_IFREG                       */
        numCharDevices,             /* FTW_F && S_IFCHR                       */
        numBlockDevices,            /* FTW_F && S_IFBLK                       */
        numFIFO,                    /* FTW_F && S_IFIFO                       */
        numSockets,                 /* FTW_F && S_IFSOCK                      */
        numDirectories,             /* FTW_D                                  */
        numUnreadableDirectories,   /* FTW_DNR                                */
        numInaccessibleFiles,       /* FTW_NS                                 */
        numSymbolicLinks,           /* FTW_SL                                 */
        numUnknown;                 /* Any other type                         */
} fileTypes;

/******************************************************************************\
* Test NFTW procedure to count file types                                      *
\******************************************************************************/

int countFileTypes(
    const char *fpath,
    const struct stat *sb,
    int typeflag,
    struct FTW *ftwbuf)
{
    switch (typeflag)
    {
        case FTW_F:
            switch (sb -> st_mode & S_IFMT)
            {
                case S_IFREG:   fileTypes.numRegularFiles++;    break;
                case S_IFCHR:   fileTypes.numCharDevices++;     break;
                case S_IFBLK:   fileTypes.numBlockDevices++;    break;
                case S_IFIFO:   fileTypes.numFIFO++;            break;
                case S_IFSOCK:  fileTypes.numSockets++;         break;
                default:        fileTypes.numUnknown++;         break;
            }
            break;
        case FTW_D:     fileTypes.numDirectories++;             break;
        case FTW_DNR:   fileTypes.numUnreadableDirectories++;   break;
        case FTW_NS:    fileTypes.numInaccessibleFiles++;       break;
        case FTW_SL:    fileTypes.numSymbolicLinks++;           break;
        default:        fileTypes.numUnknown++;                 break;
    }
    
    return FTW_CONTINUE;
}   /* countFileTypes */

/******************************************************************************\
* My implementation of ntfw().                                                 *
\******************************************************************************/

int my_nftw(
        const char *dirpath,
        int (*fn) (
            const char *fpath,
            const struct stat *sb,
            int typeflag,
            struct FTW *ftwbuf
        ),
        int nopenfd,
        int flags
    )
{

    #ifdef DEBUG
    fprintf(stderr, "my_nftw started\n");
    #endif

    /**************************************************************************\
    * Reads all entries in directory                                           *
    \**************************************************************************/
    
    void readAllEntriesInDirectory(const char * directoryName, const int level)
    {
        DIR *dir;               /* Curent directory                           */
        struct dirent *entry;   /* current entry in directory                 */
        char *fileName;         /* full name of current entry                 */
        struct stat parentStat; /* Information about parent directory         */
        struct FTW ftwbuf;
        
        #ifdef DEBUG
        fprintf(stderr, "readAllEntriesInDirectory started: directory=%s\n", directoryName);
        #endif
        
        fileName     = (char *) malloc(PATH_MAX + 1);
        if (fileName == NULL) {
            #ifdef DEBUG
            int saveErrno = errno;
            perror("readAllEntriesInDirectory: malloc for file name failed");
            errno = saveErrno;
            #endif
            return;
        }
        strcpy(fileName, directoryName);
        fileName[strlen(directoryName)] = '/';
        
        ftwbuf.base  = strlen(directoryName) + 1;
        ftwbuf.level = level;
        
        dir = opendir(directoryName);
        if (dir == NULL) {
            #ifdef DEBUG
            int saveErrno = errno;
            perror("readAllEntriesInDirectory: open directory failed");
            errno = saveErrno;
            #endif
            return;
        }
        
        for (;;)
        {
            errno = 0;
            entry = readdir(dir);
            if (entry == NULL)
                break;
            
            #ifdef DEBUG
            fprintf(stderr, "readAllEntriesInDirectory: name='%s'\n", entry -> d_name);
            #endif
            
            if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0)
                continue;
            
            strcpy(fileName + ftwbuf.base, entry -> d_name);
            
            #ifdef DEBUG
            fprintf(stderr, "readAllEntriesInDirectory: full path name='%s'\n", fileName);
            #endif
        }
        
        #ifdef DEBUG
        fprintf(stderr, "readAllEntriesInDirectory: About to close dir\n");
        #endif
        
        int saveErrno = errno;
        closedir(dir);
        
        #ifdef DEBUG
        fprintf(stderr,
                "readAllEntriesInDirectory ended: errno=%d\n",
                saveErrno
               );
        #endif

        errno = saveErrno;
        
        return;
    }   /* readAllEntriesInDirectory */

    readAllEntriesInDirectory(".", 0);

    return -1;
}   /* my_nftw */


/******************************************************************************\
* Driver procedure                                                             *
\******************************************************************************/

int main()
{
    const int directoryDepth = 20;
    int result = 0;

    fileTypes.numRegularFiles          = 0;
    fileTypes.numCharDevices           = 0;
    fileTypes.numBlockDevices          = 0;
    fileTypes.numFIFO                  = 0;
    fileTypes.numSockets               = 0;
    fileTypes.numDirectories           = 0;
    fileTypes.numUnreadableDirectories = 0;
    fileTypes.numInaccessibleFiles     = 0;
    fileTypes.numSymbolicLinks         = 0;
    fileTypes.numUnknown               = 0;
    
    #ifdef DEBUG
    fprintf(stderr, "NFTW started\n");
    #endif
    
    result = my_nftw(".", &countFileTypes, directoryDepth, FTW_ACTIONRETVAL | FTW_PHYS);
    if (errno != 0)
    {
        perror("NFTW call failed");
    }
    
    #ifdef DEBUG
    fprintf(stderr, "NFTW returned %d\n", result);
    #endif

    float totalFiles =
        fileTypes.numRegularFiles          +
        fileTypes.numCharDevices           +
        fileTypes.numBlockDevices          +
        fileTypes.numFIFO                  +
        fileTypes.numSockets               +
        fileTypes.numDirectories           +
        fileTypes.numUnreadableDirectories +
        fileTypes.numInaccessibleFiles     +
        fileTypes.numSymbolicLinks         +
        fileTypes.numUnknown;
    
    printf(
        "Num Regular files found          = %9d (%6.2f%%)\n",
        fileTypes.numRegularFiles,
        (float)fileTypes.numRegularFiles/totalFiles*100.0
    );
    printf(
        "Num Character Devices found      = %9d (%6.2f%%)\n",
        fileTypes.numCharDevices,
        (float)fileTypes.numCharDevices/totalFiles*100.0
    );
    printf(
        "Num Block Devices found          = %9d (%6.2f%%)\n",
        fileTypes.numBlockDevices,
        (float)fileTypes.numBlockDevices/totalFiles*100.0
    );
    printf(
        "Num FIFO files found             = %9d (%6.2f%%)\n",
        fileTypes.numFIFO,
        (float)fileTypes.numFIFO/totalFiles*100.0
    );
    printf(
        "Num Sockets found                = %9d (%6.2f%%)\n",
        fileTypes.numSockets,
        (float)fileTypes.numSockets/totalFiles*100.0
    );
    printf(
        "Num Directories found            = %9d (%6.2f%%)\n",
        fileTypes.numDirectories,
        (float)fileTypes.numDirectories/totalFiles*100.0
    );
    printf(
        "Num Unreadable Directories found = %9d (%6.2f%%)\n",
        fileTypes.numUnreadableDirectories,
        (float)fileTypes.numUnreadableDirectories/totalFiles*100.0
    );
    printf(
        "Num Inaccessible files found     = %9d (%6.2f%%)\n",
        fileTypes.numInaccessibleFiles,
        (float)fileTypes.numInaccessibleFiles/totalFiles*100.0
    );
    printf(
        "Num Symbolic Links found         = %9d (%6.2f%%)\n",
        fileTypes.numSymbolicLinks,
        (float)fileTypes.numSymbolicLinks/totalFiles*100.0
    );
    printf(
        "Num Unknown types found          = %9d (%6.2f%%)\n",
        fileTypes.numUnknown,
        (float)fileTypes.numUnknown/totalFiles*100.0
    );
}