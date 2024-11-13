#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void int_handler(int s)
{
  write(1, "!", 1); // 处理SIGINT时打印 "!"
}

int main()
{
  int i, j;
  sigset_t set, oset, setsave;

  // 注册SIGINT的信号处理函数
  signal(SIGINT, int_handler);

  // 初始化信号集
  sigemptyset(&set);
  sigaddset(&set, SIGINT); // 将SIGINT信号添加到set中

  // 解锁SIGINT信号
  sigprocmask(SIG_UNBLOCK, &set, &setsave);

  for (j = 0; j < 1000; j++)
  {
    // 屏蔽SIGINT信号
    sigprocmask(SIG_BLOCK, &set, &oset);

    for (i = 0; i < 5; i++)
    {
      write(1, "*", 1); // 每次循环打印 "*"
      sleep(1);         // 每次打印后暂停1秒
    }

    write(1, "\n", 1); // 每5次循环后换行

    // 恢复之前的信号掩码
    sigprocmask(SIG_SETMASK, &oset, NULL);

    // 等待信号，直到SIGINT信号到来
    pause();
  }

  // 恢复最初的信号掩码
  sigprocmask(SIG_SETMASK, &setsave, NULL);
  exit(0); // 正常退出程序
}
