/******************************************************************************\
* Exercise 18-7:                                                               *
* Uses nftw() to traverse a directory tree and count the number of various     *
* types. Counts and percentages are to be printed out.                         *
\******************************************************************************/

#define _GNU_SOURCE

#include <errno.h>
#include <ftw.h>
#include <stdio.h>

/******************************************************************************\
* From the NFTW(3) MAN page:                                                   *
*   The typeflag argument passed to fn() is an integer that has one of the     *
*   following values:                                                          *
*                                                                              *
*   FTW_F  fpath is a regular file.                                            *
*                                                                              *
*   FTW_D  fpath is a directory.                                               *
*                                                                              *
*   FTW_DNR                                                                    *
*          fpath is a directory which can't be read.                           *
*                                                                              *
*   FTW_DP fpath  is a directory, and FTW_DEPTH was specified in flags.  (If   *
*          FTW_DEPTH was not specified in flags, then directories will always  *
*          be visited with typeflag set to FTW_D.)  All of the files and       *
*          subdirectories within fpath have been processed.                    *
*                                                                              *
*   FTW_NS The stat(2) call failed on fpath, which is not a symbolic link.     *
*          The probable cause for this is that the caller had read permission  *
*          on the parent directory, so that the filename  fpath could be seen, *
*          but did not have execute permission, so that the file could not be  *
*          reached for stat(2).  The contents of the buffer pointed to by sb   *
*          are undefined.                                                      *
*                                                                              *
*   FTW_SL fpath is a symbolic link, and FTW_PHYS was set in flags.            *
*                                                                              *
*   FTW_SLN                                                                    *
*          fpath  is  a  symbolic  link  pointing to a nonexistent file.       *
*          (This occurs only if FTW_PHYS is not set.)  On most implementations,*
*          in this case the sb argument passed to fn() contains information    *
*          returned by performing lstat(2) on the symbolic link.  For the      *
*          details on Linux, see BUGS.                                         *
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
    
    result = nftw(".", &countFileTypes, directoryDepth, FTW_ACTIONRETVAL | FTW_PHYS);
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