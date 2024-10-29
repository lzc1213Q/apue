
// 实时信号不会丢失
/*
在Unix / Linux系统中，实时信号（real - time signals）是一种特殊的信号类型，用于在进程之间传递信息。相比于传统的信号，实时信号具有一些特性，使得在一定条件下，它们的丢失风险相对较小。以下是一些关键因素，解释为什么Unix C中的实时信号能够减少丢失的情况：
1. 优先级和排队
优先级：实时信号的优先级高于传统信号（如SIGKILL和SIGSTOP），这意味着当一个进程接收到实时信号时，它的处理优先级较高。
排队机制：Unix/Linux允许实时信号按照顺序排队。每个实时信号都可以被视为一个消息队列，信号可以按照发送的顺序排队，这样可以避免信号的覆盖和丢失。实时信号的编号范围从SIGRTMIN到SIGRTMAX，并且可以发送多达32个信号实例。
2. 信号处理函数
定制的信号处理：可以为每种实时信号指定不同的信号处理函数，这允许开发者实现自定义的逻辑以处理接收到的信号。通过在信号处理程序中实现高效的逻辑，可以减少丢失的可能性。
阻塞与忽略：进程可以在处理实时信号时选择阻塞或忽略其他信号，从而减少信号冲突，确保实时信号能被及时处理。
3. 信号集的使用
信号掩码：使用信号掩码来管理信号的接收，允许进程在处理特定信号时屏蔽其他信号。这样可以避免在信号处理过程中发生的竞态条件，确保实时信号被准确处理。
4. 多线程支持
线程和实时信号：在多线程程序中，实时信号可以被发送到特定的线程，这使得信号的处理可以更为精细和高效。这样可以降低由于线程调度引起的信号丢失的可能性。
5. 数据传输能力
附带数据：实时信号可以附带额外的数据（如通过sigqueue()函数发送信号时），这使得信号不仅仅是简单的通知，还能传递具体信息，从而减少需要多次发送信号的情况。
6. 合理设计和优化
合理的信号设计：在系统设计时，合理安排信号的使用方式和发送频率，避免信号过载，从而减少信号丢失的风险。
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void my_handler(int s)
{
    write(1, "!", 1);
}
int main()
{
    int i, j;

    sigset_t set, oset, setsave;

    // signal(SIGINT,SIG_IGN);
    signal(SIGRTMAX, my_handler);
    sigemptyset(&set);
    sigaddset(&set, SIGRTMAX);
    sigprocmask(SIG_UNBLOCK, &set, &setsave);
    sigprocmask(SIG_BLOCK, &set, &oset);
    for (j = 0; j < 1000; j++)
    {
        for (i = 0; i < 5; i++)
        {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        sigsuspend(&oset);
    }
    sigprocmask(SIG_SETMASK, &setsave, NULL);
    exit(0);
}
