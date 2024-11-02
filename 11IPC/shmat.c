#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

// 申请的共享内存大小，单位是字节
#define MEMSIZE 1024

int main()
{
    char *str;
    pid_t pid;
    int shmid;

    // 有亲缘关系的进程 key 参数可以使用 IPC_PRIVATE 宏，并且创建共享内存 shmflg 参数不需要使用 IPC_CREAT 宏
    shmid = shmget(IPC_PRIVATE, MEMSIZE, 0600);
    if (shmid < 0)
    {
        perror("shmget()");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) // 子进程
    {
        // 关联共享内存
        str = shmat(shmid, NULL, 0);
        if (str == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }
        // 向共享内存写入数据
        strcpy(str, "Hello!");
        // 分离共享内存
        shmdt(str);
        // 无需释放共享内存
        exit(0);
    }
    else // 父进程
    {
        // 等待子进程结束再运行，因为需要读取子进程写入共享内存的数据
        wait(NULL);
        // 关联共享内存
        str = shmat(shmid, NULL, 0);
        if (str == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }
        // 直接把共享内存中的数据打印出来
        puts(str);
        // 分离共享内存
        shmdt(str);
        // 释放共享内存
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }

    exit(0);
}
