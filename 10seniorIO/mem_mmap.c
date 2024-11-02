#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

#define MEMSIZE 1024

int main(void)
{
    char *str;
    pid_t pid;

    // 这里在 flags 中添加 MAP_ANONYMOUS，为制作共享内存做准备
    str = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (str == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    // 创建子进程，父子进程使用共享内存进行通信
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) // 子进程向共享内存中写入数据
    {
        strcpy(str, "Hello!");
        munmap(str, MEMSIZE); // 注意，虽然共享内存是在 fork(2) 之前创建的，但是 fork(2) 的时候子进程也拷贝了一份，所以子进程使用完毕之后也要解除映射
        exit(0);
    }
    else // 父进程从共享内存中读取子进程写入的数据
    {
        wait(NULL);           // 保障子进程先运行起来，因为就算父进程先运行了也会在这里阻塞等待
        puts(str);            // 把从共享内存中读取出来的数据打印出来
        munmap(str, MEMSIZE); // 不要忘记解除映射
        exit(0);
    }

    exit(0);
}
