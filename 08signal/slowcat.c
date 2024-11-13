#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define CPS 10
#define BUFSIZE CPS

static volatile int loop = 0;

static void alrm_handler(int s)
{
    alarm(1);
    loop = 1;
}
int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret;
    int pos;
    if (argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }

    signal(SIGALRM, alrm_handler);
    alarm(1);

    sfd = open(argv[1], O_RDONLY);
    if (sfd < 0)
    {
        perror("open_sfd()");
        exit(1);
    }

    while (1)
    {
        while (!loop) // 等待 signal 触发，设置 loop 为 1
            pause();  // 暂停进程，直到信号发生
        loop = 0;     // 重置 loop 为 0，避免下一次读数据
        len = read(sfd, buf, BUFSIZE);
        if (len < 0)
        {
            perror("read()");
            break;
        }
        if (len == 0) // 文件结束
            break;
        pos = 0;
        while (len > 0)
        {
            ret = write(dfd, buf + pos, len);
            if (ret < 0)
            {
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
    }
    close(sfd);
    exit(0);
}
