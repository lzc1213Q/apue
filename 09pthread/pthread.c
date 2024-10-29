
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void *func(void *p)
{
    puts("Thread is working.");

    sleep(10); // 延时是为了方便我们使用 ps(1) 命令验证线程是否被创建了

    pthread_exit(NULL);
//    return NULL;
}

int main()
{
    pthread_t tid;
    int err;

    puts("Begin!");

    // 创建线程
    err = pthread_create(&tid,NULL,func,NULL);
    if(err)
    {
        fprintf(stderr,"pthread_create():%s\n",strerror(err));
        exit(1);
    }

    // 为线程收尸
    pthread_join(tid,NULL);

    puts("End!");

    exit(0);
}