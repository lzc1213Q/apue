#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 1024
int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret;
    int pos;
    if(argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }

    sfd = open(argv[1], O_RDONLY);
    if(sfd < 0) 
    {
        perror("open_sfd()");
        exit(1);
    }

   
    while(1)
    {
      len =  read(sfd,buf,BUFSIZE);
        if(len <0)
        {
            perror("read()");
            break;
        }
        if(len == 0)
        break;
        pos = 0;
        while(len > 0)
        {
            ret  = write(dfd,buf + pos ,len);
            if(ret < 0)
            {
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
    }
    close(sfd);
    exit(0); 
}
