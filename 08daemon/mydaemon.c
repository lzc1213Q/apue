#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#define FNAME "tmp/out"
//实现守护进程
static int darmonize(void)
{
    pid_t pid;
    int fd;
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        return -1;
    }
    if(pid > 0)
    {
        exit(0);
    }
    fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
        perror("open()");

        return -1;
    }
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    if(fd > 2)
        close(fd);
    setsid();
    chdir("/");
}
 int main()
 {
     FILE *fp;
     int i;
     openlog("mydaemon",LOG_PID,LOG_DAEMON);
    if(darmonize())
    {
        syslog(LOG_ERR,"daemonise() failed!");
        exit(1);
    }
    else
    {
        syslog(LOG_INFO,"deaemonnize() successede! ");
    }
    fp = fopen(FNAME, "w");
    if(fp == NULL)
    {
        syslog(LOG_ERR,"fopen():%s",strerror(errno));
        exit(1);
    }
    syslog(LOG_INFO,"was fopened.",FNAME);
    for(i = 0;  ; i++)
    {
        fprintf(fp,"%d\n",i);
        syslog(LOG_DEBUG,"%d is printed.",i);
        fflush(NULL);
        sleep(1);
    }
    fclose(fp);
    closelog();
     exit(0);
 }
