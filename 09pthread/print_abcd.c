#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define THRNUM 4
static pthread_mutex_t mut[THRNUM];
// 循环abcde
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
        
        pthread_mutex_lock(mut + n);
        write(1, &ch, 1);
        pthread_mutex_unlock(mut + next(n));
    }
    pthread_exit(NULL);
}
int main()
{
    int i, err;
    pthread_t tid[THRNUM];
    pthread_mutex_init(mut + i, NULL);
    pthread_mutex_lock(mut + i);
    for (i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid + i, NULL, thr_func, (void *)i);
        if (err)
        {
            fprintf(stderr, "pthread_create():%s\n", strerror(err));
            exit(1);
        }
    }
    pthread_mutex_unlock(mut);
    alarm(5);
    for (i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[1], NULL);
    }
    for (i = 0; i < THRNUM; i++)
    {
        pthread_mutex_destroy(mut + i);
    }
}
