#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
static int ftype(const char *fname)
{
    struct stat startres;
    if(stat(fname,&startres) < 0)
    {
        perror("stat()");
        exit(1);
    }
     if( S_ISREG(startres.st_mode) )
           return '-';//普通文件
        else if( S_ISDIR(startres.st_mode) )
            return 'd';//目录文件
        else if( S_ISCHR(startres.st_mode) )
            return 'c';//字符设备文件
        else if( S_ISBLK(startres.st_mode) )
            return 'b';//块设备文件
        else if( S_ISFIFO(startres.st_mode) )
            return 'p';//管道文件
        else if( S_ISLNK(startres.st_mode) )
            return 'l';//符号链接文件
        else if( S_ISSOCK(startres.st_mode) )
            return 's';//套接字文件
        else
            return '?';

}
int main(int argc, char **argv)
{
    int fd;
      if(argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }
    printf("%c\n", ftype(argv[1]));

    exit(0);
}