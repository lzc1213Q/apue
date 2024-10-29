#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include "mytbf.h"

#define CPS 10
#define BUFSIZE 1024
#define BURST 100
static volatile int token = 0; 

int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret;
    int pos, size;
    mytbf_t *tbf;

    if(argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }
    tbf = mytbf_init(CPS,BURST);
    if(tbf == NULL)
    {
        fprintf(stderr,"mytbf_init() failed! \n");
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
        size = mytbf_fetchtoken(tbf,BUFSIZE);
        if(size < 0)
        {
            fprintf(stderr,"myrbf_fetchtoken(): %s\n", strerror(-size)); 
            exit(1);
        }
        len =  read(sfd,buf,size);
        if(len <0)
        {
            perror("read()");
            break;
        }
        if(len == 0)
        break;
        if(size - len > 0)
        {
            mytbf_returntoken(tbf, size - len);
        }
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
    mytbf_destory(tbf);
    exit(0); 
}
