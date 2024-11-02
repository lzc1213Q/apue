#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

#define PROCNUM 20
#define FNAME "/tmp/out"
#define BUFSIZE 1024

static void func_add()
{
    FILE *fp;
    int fd;
    char buf[BUFSIZE];

    fp = fopen(FNAME, "r+");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    fd = fileno(fp);
    if (fd < 0)
    {
        perror("fd");
        exit(1);
    }

    // 使用之前先锁定
    lockf(fd, F_LOCK, 0);

    fgets(buf, BUFSIZE, fp);
    rewind(fp); // 把文件位置指针定位到文件首
    sleep(1);   // 放大竞争
    fprintf(fp, "%d\n", atoi(buf) + 1);
    fflush(fp);

    // 使用之后释放锁
    lockf(fd, F_ULOCK, 0);

    fclose(fp);

    return;
}

int main(void)
{
    int i;
    pid_t pid;

    for (i = 0; i < PROCNUM; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if (pid == 0) // child
        {
            func_add();
            exit(0);
        }
    }

    for (i = 0; i < PROCNUM; i++)
        wait(NULL);

    exit(0);
}
