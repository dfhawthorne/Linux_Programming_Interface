/******************************************************************************\
*                                                                              *
* NAME                                                                         *
*        cp - copy files and directories                                       *
*                                                                              *
* Copies sparse files.                                                         *
\******************************************************************************/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    /**************************************************************************\
    * Check passed arguments                                                   *
    \**************************************************************************/
    
    if (argc < 3) {
        fprintf(stderr, "Usage %s <in_file> <out_file>\n", argv[0]);
        exit(1);
    }
    
    /**************************************************************************\
    * Open input file, get vital statistics, and allocate input buffer based   *
    * on physical block size.                                                  *
    \**************************************************************************/
    
    int iFile = open(argv[1], O_RDONLY);
    if (iFile == -1) {
        perror("CP: Unable to open input file");
        exit(1);
    }
    
    struct stat iFileStat;
    int rc = fstat(iFile, &iFileStat);
    if (rc == -1) {
        perror("CP: Unable to stat input file");
        close(iFile);
        exit(1);
    }
    
    size_t bufferSize = iFileStat.st_blksize;
    void *iBuffer = malloc(bufferSize);
    if (iBuffer == NULL) {
        perror("CP: Unable to allocate input buffer");
        close(iFile);
        exit(1);
    }
    
    /**************************************************************************\
    * Open output file overwriting any existing data, and allowing read access *
    * by anyone but restricting write access to the current user.              *
    \**************************************************************************/
    
    int oFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (oFile == -1) {
        perror("CP: Unable to open output file");
        free(iBuffer);
        close(iFile);
        exit(1);
    }
    
    /**************************************************************************\
    * Copy the input file to the output file                                   *
    * If we encounter a physical block that is all zeroes, assume that we are  *
    * in a sparse file.                                                        *
    \**************************************************************************/
    
    int exitStatus = 0;
    int numBytes = 0;
    while ((numBytes = read(iFile, iBuffer, bufferSize)) > 0) {
        char *bp = (char *)iBuffer;
        int i;
        for (int i = 0; (i < numBytes) && (*bp == '\0'); i++, bp++)
            ;
        if (i == numBytes) {
            if (lseek(oFile, numBytes, SEEK_CUR) == -1) {
                perror("CP: Unable to seek in output file");
                exitStatus = 1;
                break;
            }
        } else {
            if (write(oFile, iBuffer, numBytes) == -1) {
                perror("CP: Unable to write to output file");
                exitStatus = 1;
                break;
            }
        }
    }
    
    if (numBytes == -1) {
        perror("CP: Unable to read from input file");
        exitStatus = 1;
    }
    
    /**************************************************************************\
    * Close all files and release all dynamic memory                           *
    \**************************************************************************/

    free(iBuffer);
    close(iFile);
    close(oFile);
    
    exit(exitStatus);
}
