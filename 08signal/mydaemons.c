// 一个简单的守护进程。让我来为你解释和分析一下代码的工作流程和几个关键部分。

// 功能简介：
// 代码的目标是创建一个守护进程，守护进程会在后台运行，并周期性地将一个递增的整数值写入一个文件（tmp/out），同时将一些信息写入系统日志中。

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#define FNAME "tmp/out"
// 实现守护进程

static FILE *fp;
static int darmonize(void)
{
    pid_t pid;
    int fd;
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        return -1;
    }
    if (pid > 0)
    {
        exit(0);
    }
    fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        perror("open()");

        return -1;
    }
    // 重定向标准输入、标准输出和标准错误输出到 /dev/null，防止守护进程产生任何输出到控制台。
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if (fd > 2)
        close(fd);
    // 创建新的会话，使得守护进程脱离终端。
    setsid();
    // 更改工作目录到根目录 /，防止守护进程锁定任何特定的目录。
    chdir("/");
}
// 处理退出信号
static void deamon_exit(int s)
{
    fclose(fp);
    closelog();
    exit(0);
}
int main()
{

    int i;
    // 设置对 SIGINT、SIGQUIT、SIGTERM 等信号的处理，确保守护进程收到这些信号时能够正确清理资源并退出。
    struct sigaction sa;
    sa.sa_handler = deamon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    if (darmonize())
    {
        syslog(LOG_ERR, "daemonise() failed!");
        exit(1);
    }
    else
    {
        syslog(LOG_INFO, "deaemonnize() successede! ");
    }
    fp = fopen(FNAME, "w");
    if (fp == NULL)
    {
        syslog(LOG_ERR, "fopen():%s", strerror(errno));
        exit(1);
    }
    syslog(LOG_INFO, "was fopened.", FNAME);
    for (i = 0;; i++)
    {
        fprintf(fp, "%d\n", i);
        syslog(LOG_DEBUG, "%d is printed.", i);
        //  强制刷新文件缓冲区，确保数据及时写入文件。
        fflush(NULL);
        sleep(1);
    }
    fclose(fp);
    closelog();
    exit(0);
}
