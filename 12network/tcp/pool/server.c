// 这是一个实现基于进程池的 TCP 服务器的 C 代码，目的是管理多个工作进程来处理客户端连接。每个工作进程负责与一个客户端进行通信，并且进程池根据负载动态调整工作进程的数量。
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>

#include "proto.h"

#define SIG_NOTIFY SIGUSR2 // 定义了一个信号 SIGUSR2，用来通知父进程工作进程的状态。

#define MINSPACESERVER 5
#define MAXSPACESERVER 10
#define MAXCLINETS 20

#define IPSIZE 1024
#define BUFSIZE 1024

enum
{
    STATE_IDLE = 0, // idle 空闲的
    STATE_BUSY      // 忙态
};

struct server_st
{
    pid_t pid; // 进程id
    int state; // 进程状态
};

static struct server_st *serverpool;
static int idle_count = 0, busy_count = 0;

// socket相关全局变量
static int sfd;
// 是一个空的信号处理函数，用于处理 SIG_NOTIFY 信号
static void handle(int sig)
{
    return;
}

static void server_job(int pos)
{
    int newsd;
    int ppid;

    struct sockaddr_in raddr; // remote addr
    char ip[IPSIZE];

    socklen_t raddr_len = sizeof(raddr);

    ppid = getppid(); // 父进程

    while (1)
    {
        serverpool[pos].state = STATE_IDLE;
        kill(ppid, SIG_NOTIFY); // 通知父进程该进程空闲

        newsd = accept(sfd, (void *)&raddr, &raddr_len); // 接收客户端连接
        if (newsd < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            else
            {
                perror("accept()");
                exit(1);
            }
        }

        serverpool[pos].state = STATE_BUSY;              // 处理客户端请求，更新为忙碌状态
        kill(ppid, SIG_NOTIFY);                          // 通知父进程该进程忙碌
        inet_ntop(AF_INET, &raddr.sin_addr, ip, IPSIZE); // 获取客户端 IP
        // 向客户端发送时间戳
        char buf[BUFSIZE];
        int pkglen = 0;

        pkglen = sprintf(buf, FMT_STAMP, (long long)time(NULL));

        if (send(newsd, buf, pkglen, 0) < 0)
        {
            perror("send()");
            exit(1);
        }
        // 关闭客户端连接
        close(newsd);
        // 每处理一个连接后休眠 5 秒
        sleep(5);
    }
}
// 负责向进程池中添加一个新的工作进程。如果当前进程池已经满了，则返回失败。
static int add_one_server()
{
    int slot;
    pid_t pid;

    if (idle_count + busy_count >= MAXCLINETS)
    {
        return -1; // 如果已经达到最大客户端数，不能再添加
    }

    for (slot = 0; slot < MAXCLINETS; slot++)
    {
        if (serverpool[slot].pid == -1) // 找到一个空闲槽
        {
            break;
        }
    }
    serverpool[slot].state = STATE_IDLE;
    pid = fork(); // 创建子进程
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    // 子进程
    if (pid == 0)
    {
        server_job(slot); // 子进程执行工作
        exit(0);
    }
    // 父进程
    else
    {
        serverpool[slot].pid = pid; // 父进程记录子进程 PID
        idle_count++;               // 空闲进程数加 1
    }
    return 0;
}

static int del_one_server()
{
    int slot;
    if (idle_count == 0)
    {
        return -1; // 如果没有空闲的进程，不能删除
    }

    for (slot = 0; slot < MAXCLINETS; slot++)
    {
        if (serverpool[slot].pid != -1 && serverpool[slot].state == STATE_IDLE)
        {
            kill(serverpool[slot].pid, SIGTERM); // 发送信号终止空闲进程
            serverpool[slot].pid = -1;           // 释放该槽
            idle_count--;                        // 空闲进程数减 1
            break;
        }
    }
    return 0;
}

static void scan_pool()
{
    int idle = 0, busy = 0;
    for (int i = 0; i < MAXCLINETS; i++)
    {
        if (serverpool[i].pid == -1)
        {
            continue;
        }
        if (kill(serverpool[i].pid, 0)) // 检查进程是否存活
        {
            serverpool[i].pid = -1; // 进程已退出，清理
            continue;
        }
        // 统计进程池的状态
        if (serverpool[i].state == STATE_IDLE)
            idle++;
        else if (serverpool[i].state == STATE_BUSY)
            busy++;
        else
        {
            fprintf(stderr, "未知状态!\n");
            abort();
        }
    }
    idle_count = idle;
    busy_count = busy;
}

int main()
{
    struct sigaction sa, osa;

    sa.sa_handler = SIG_IGN; // 忽略父进程的资源回收信号
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT; // 让子进程结束后自行消亡,不会变成僵尸状态
    sigaction(SIGCLD, &sa, &osa);

    sa.sa_handler = handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &osa);

    // 屏蔽信号
    sigset_t sigset, oldsigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &sigset, &oldsigset);

    // 共享内存地址映射
    serverpool = mmap(NULL, sizeof(struct server_st) * MAXCLINETS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (serverpool == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    // 初始化进程池
    for (int i = 0; i < MAXCLINETS; i++)
    {
        serverpool[i].pid = -1;
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0 /*IPPROTO_TCP*/);
    if (sfd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 对指定Socket sd 的SOL_SOCKET层面 的 SO_REUSEADDR属性进行设置，即打开该属性，如果发现 端口没有释放，该属性会马上释放该端口，并且让我们绑定成功。
    int val = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    struct sockaddr_in laddr;                       // local addr
    laddr.sin_family = AF_INET;                     // 指定协议
    laddr.sin_port = htons(atoi(SERVERPORT));       // 指定网络通信端口
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr); // IPv4点分式转二进制数

    if (bind(sfd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    if (listen(sfd, 1024) < 0)
    { // 全连接数量
        perror("listen()");
        exit(1);
    }
    // 如果进程数量少于最小进程数添加一个socket接收进程
    for (int i = 0; i < MINSPACESERVER; i++)
    {

        add_one_server();
    }

    while (1)
    {
        sigsuspend(&oldsigset);

        scan_pool(); // 扫描进程池

        // 管理进程池
        if (idle_count > MAXSPACESERVER)
        {
            for (int i = 0; i < (idle_count - MAXSPACESERVER); i++)
            {
                del_one_server();
            }
        }
        else if (idle_count < MINSPACESERVER)
        {
            for (int i = 0; i < (MINSPACESERVER - idle_count); i++)
            {
                add_one_server();
            }
        }

        // print the pool
        for (int i = 0; i < MAXCLINETS; i++)
        {
            if (serverpool[i].pid == -1)
            {
                printf(" ");
            }
            else if (serverpool[i].state == STATE_IDLE)
            {
                printf(".");
            }
            else if (serverpool[i].state == STATE_BUSY)
            {
                printf("X");
            }
            fflush(NULL);
        }
        printf("\n");
    }

    close(sfd);
    sigprocmask(SIG_SETMASK, &oldsigset, NULL);

    exit(0);
}