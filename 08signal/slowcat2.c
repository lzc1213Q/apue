#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#define CPS 10
#define BUFSIZE CPS
#define BURST 100

static volatile int token = 0; 

static void alrm_handler(int s)
{
   // alarm(1);
    token++;
    if(token > BURST)
        token = BURST;
    
}
int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret;
    int pos;
    struct itimerval itv;
    if(argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }
    
    signal(SIGALRM,alrm_handler);
   // alarm(1);
   //利用setitimer 函数替换alarm setitimer函数为原子操作。
   itv.it_interval.tv_sec = 1;
   itv.it_interval.tv_usec = 0;
   itv.it_value.tv_sec = 1;
   itv.it_value.tv_usec = 0;
   if(setitimer(ITIMER_REAL, &itv,NULL) < 0)
   {
       perror("sititimer()");
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
        while(token <= 0)
            pause();
        token--;
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
