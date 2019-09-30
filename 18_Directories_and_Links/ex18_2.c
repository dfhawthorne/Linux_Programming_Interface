/********************************************************************************
* Exercise 18-2:                                                                *
********************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

int
main()
{
    int fd;
    
    mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
    errMsg("mkdir");
    chdir("test");
    errMsg("chdir");
    fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    errMsg("open");
    symlink("myfile", "../mylink");
    errMsg("symlink");
    chmod("../mylink", S_IRUSR);
    errExit("err18_2");
}
