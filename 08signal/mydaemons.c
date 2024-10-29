#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#define FNAME "tmp/out"
//实现守护进程

static FILE *fp;
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
static void deamon_exit(int s)
{
    fclose(fp);
    closelog();
    exit(0);
}
 int main()
 {
    
     int i;
     struct sigaction sa;
     sa.sa_handler = deamon_exit;
     sigemptyset(&sa.sa_mask);
     sigaddset(&sa.sa_mask,SIGQUIT);
     sigaddset(&sa.sa_mask,SIGTERM);
     sigaddset(&sa.sa_mask,SIGINT);
     sa.sa_flags = 0;
     sigaction(SIGINT,&sa,NULL);
     sigaction(SIGQUIT,&sa,NULL);
     sigaction(SIGTERM,&sa,NULL);

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
