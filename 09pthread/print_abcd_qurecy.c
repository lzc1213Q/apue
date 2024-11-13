#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define THRNUM 4
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int num = 0;
static int next(int a)
{
    if (a + 1 == THRNUM)
        return 0;
    return a + 1;
}
static void *thr_func(void *p)
{
    int n = (int)p;
    int ch = n + 'a';

    while (1)
    {
        pthread_mutex_lock(&mut); // 加锁
        while (num != n)          // 如果当前线程不该运行，等待
        {
            pthread_cond_wait(&cond, &mut);
        }

        write(1, &ch, 1);
        num = next(num);               // 更新控制变量
        pthread_cond_broadcast(&cond); // 广播通知其他线程
        pthread_mutex_unlock(&mut);    // 解锁
    }
    pthread_exit(NULL);
}
int main()
{
    int i, err;
    pthread_t tid[THRNUM];

    for (i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid + i, NULL, thr_func, (void *)i);
        if (err)
        {
            fprintf(stderr, "pthread_create():%s\n", strerror(err));
            exit(1);
        }
    }
    alarm(5);
    for (i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[1], NULL);
    }
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);

    exit(0);
}
