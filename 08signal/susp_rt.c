#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void my_handler(int s)
{
    write(1, "!", 1); // 处理SIGRTMAX信号时打印 "!"
}

int main()
{
    int i, j;

    sigset_t set, oset, setsave;

    // 设置SIGRTMAX信号的处理函数
    signal(SIGRTMAX, my_handler);

    // 初始化信号集
    sigemptyset(&set);
    sigaddset(&set, SIGRTMAX); // 将SIGRTMAX信号添加到set中

    // 阻塞SIGRTMAX信号并保存当前信号掩码到oset
    sigprocmask(SIG_BLOCK, &set, &oset);

    for (j = 0; j < 1000; j++)
    {
        for (i = 0; i < 5; i++)
        {
            write(1, "*", 1); // 每次循环打印 "*"
            sleep(1);         // 每次打印后暂停1秒
        }

        write(1, "\n", 1); // 每5次循环后换行

        // 挂起进程，直到接收到解除屏蔽的信号（在此为 SIGRTMAX）
        sigsuspend(&oset);
    }

    // 恢复最初的信号掩码
    sigprocmask(SIG_SETMASK, &setsave, NULL);

    exit(0); // 正常退出程序
}
