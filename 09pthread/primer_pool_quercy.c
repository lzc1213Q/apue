#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM 4
static int num = 0;
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_num = PTHREAD_COND_INITIALIZER;
static void *thr_prime(void *p)
{
    int i, j, mark;

    i = (int)p;
    while (1)
    {
        pthread_mutex_lock(&mut_num);
        // 加锁，进入临界区，确保对共享变量 num 的访问是线程安全的。
        while (num == 0)
        {
            // pthread_mutex_unlock(&mut_num);
            // sched_yield();
            // pthread_mutex_lock(&mut_num);
            pthread_cond_wait(&cond_num, &mut_num);
            // 如果 num 等于 0，表示主线程还没有分配任务给该线程，线程会等待。
        }
        // 如果 num 为 -1，表示主线程通知所有线程退出，线程应当终止。
        if (num == -1)
        {
            // 跳转到临界区外先解锁在跳转。
            pthread_mutex_unlock(&mut_num);
            break;
        }
        i = num;
        // 将当前任务（num）保存到变量 i 中，并将 num 设置为 0，表示当前任务已被分配。
        num = 0;
        // 清空 num，准备接受主线程的下一次任务。
        pthread_cond_broadcast(&cond_num);
        // 广播通知所有等待的线程，当前线程已经完成了工作，其他线程可以继续工
        pthread_mutex_unlock(&mut_num);
        // 释放互斥锁，允许其他线程访问共享资源。
        mark = 1;
        //  是质数，标记为 1。
        for (j = 2; j < i / 2; j++)
        {
            if (i % j == 0)
            {
                mark = 0;
                break;
            }
        }
        if (mark)
            printf("[%d]%d is a primer\n", (int)p, i);
    }
    pthread_exit(NULL);
}
int main()
{
    int i, err;
    pthread_t tid[THRNUM];
    for (i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid + i, NULL, thr_prime, (void *)i);
        if (err)
        {
            fprintf(stderr, "pthread_create():%s\n", strerror(err));
            exit(1);
        }
    }
    for (i = LEFT; i < RIGHT; i++)
    {
        // 加锁，进入临界区。
        pthread_mutex_lock(&mut_num);
        while (num != 0)
        {
            // pthread_mutex_unlock(&mut_num);
            // sched_yield();
            // pthread_mutex_lock(&mut_num);
            pthread_cond_wait(&cond_num, &mut_num);
            // 等待其他线程完成任务，直到可以分配下一个任务。
        }
        num = i;
        // 将当前数字分配给 num，准备分配给某个线程。
        pthread_cond_signal(&cond_num);
        // 通知一个等待的线程开始处理这个数字。
        pthread_mutex_unlock(&mut_num);
        // 释放锁，允许其他线程操作。
    }

    pthread_mutex_lock(&mut_num);
    while (num != 0)
    {
        pthread_mutex_unlock(&mut_num);
        sched_yield();
        pthread_mutex_lock(&mut_num);
    }
    pthread_cond_broadcast(&cond_num);
    num = -1;
    pthread_mutex_unlock(&mut_num);
    for (i = 0; i < THRNUM; i++)
        pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&mut_num);
    pthread_cond_destroy(&cond_num);
    exit(0);
}
