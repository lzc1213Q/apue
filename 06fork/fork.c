#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    fflush(NULL);//在创建进程之前一定要刷新缓存区
    printf("Begin!\n",getpid());
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0 )
    {
        //child
        printf("[%d]:Child is working!\n",getpid());
    }
     else
    {
        //parent
        printf("[%d]:Parent is working!\n",getpid());
    }
    printf("[%d]End!\n",getpid());
    exit(0);
}

