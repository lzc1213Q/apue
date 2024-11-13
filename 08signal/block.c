
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void int_handler(int s)
{
    write(1, "!", 1);
}
int main()
{
    int i, j;

    sigset_t set, oset, setsave;

    // signal(SIGINT,SIG_IGN);
    // 响应ctrl+c 信号中断
    signal(SIGINT, int_handler);
    // 初始化信号变量
    sigemptyset(&set);
    // 将Ctrl+c信号加入信号集
    sigaddset(&set, SIGINT);
    //解锁
    sigprocmask(SIG_UNBLOCK, &set, &setsave);
    for (j = 0; j < 1000; j++)
    {
        // 加锁
        sigprocmask(SIG_BLOCK, &set, NULL);
        for (i = 0; i < 5; i++)
        {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        
        sigprocmask(SIG_SETMASK, &oset, NULL);
    }
    sigprocmask(SIG_SETMASK, &setsave, NULL);
    exit(0);
}