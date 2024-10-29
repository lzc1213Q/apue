#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

#include "mytbf.h"
static struct mytbf_st* job[MYTBF_MAX];
static int inited = 0;
/* 用来保护令牌桶数组竞争的互斥量 */
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
/* 添加令牌的线程 ID */
static pthread_t tid;
/* 初始化添加令牌的线程 */
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
    pthread_mutex_t mut; //用来保护令牌竞争的互斥量
    pthread_cond_t cond;
};


static int get_free_pos_unlocked(void)
{
    int i;
    for(i = 0; i < MYTBF_MAX; i++ )
    {
        if(job[i] == NULL)
        return i;
    }
    return -1;
}
static void *thr_alrm(void *p)
{
    int i;
    
    while(1)
    {
        pthread_mutex_lock(&mut_job);
        for(i = 0; i < MYTBF_MAX; i++)
        {
            if(job[i] != NULL)
            {
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token +=job[i]->cps;
                if(job[i]->token > job[i]->burst)
                {
                    job[i]->token = job[i]->burst; 
                }
                // 令牌添加完毕之后，通知所有等待使用令牌的线程准备抢锁
                pthread_cond_broadcast(&job[i]->cond);
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }  
}
static void module_unload(void)//释放令牌桶
{
    int i;
    pthread_cancel(tid);
    pthread_join(tid,NULL);

    pthread_mutex_lock(&mut_job);
    for(i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL)
        {
            pthread_mutex_destroy(&job[i]->mut);
            pthread_cond_destroy(&job[i]->cond);
            free(job[i]);
        }
    }
    pthread_mutex_unlock(&mut_job);
    pthread_mutex_destroy(&mut_job);
}
static void module_load(void)//开启令牌桶
{
    int err;
    err = pthread_create(&tid,NULL,thr_alrm,NULL);
    if(err)
    {
        fprintf(stderr,"pthread_create():%s\n", strerror(err));
        exit(1);
    }
    atexit(module_unload);
}


mytbf_t *mytbf_init(int cps,int burst)
{
    struct mytbf_st *me;
    int pos;
    pthread_once(&init_once,module_load);

    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    pthread_mutex_init(&me->mut,NULL);
    pthread_cond_init(&me->cond,NULL);

    pthread_mutex_lock(&mut_job);
    pos = get_free_pos_unlocked();
    if(pos < 0)
    {
         // 带锁跳转不要忘记先解锁再跳转
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
   
    me->pos = pos;
    job[pos] = me;
    pthread_mutex_unlock(&mut_job);

    return me;
}
static int min(int a,int b)
{
    if(a < b)
        return a;
    return b;
}
int mytbf_fetchtoken(mytbf_t *ptr,int size)
{
    struct mytbf_st *me = ptr;
    int n;
    if(size <= 0)
        return -EINVAL;
    // 令牌数量不足，等待令牌被添加进来
    pthread_mutex_lock(&me->mut);
    while(me->token <= 0)
    {
        // 先解锁，出让调度器让别人先跑起来，然后再抢锁检查令牌是否够用
        // pthread_mutex_unlock(&me->mut);
        // sched_yield();
        // pthread_mutex_lock(&me->mut);
        pthread_cond_wait(&me->cond, &me->mut);
    }
    n = min(me->token,size);
    me->token -= n;
    pthread_mutex_unlock(&me->mut);
    return n;

    
}

int mytbf_returntoken(mytbf_t *ptr,int size)
{
    struct mytbf_st *me = ptr;

    if(size <= 0)
        return -EINVAL;
    pthread_mutex_lock(&me->mut);   
    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut); 
    
    return size;
    
}
int mytbf_destory(mytbf_t *ptr)
{
    struct mytbf_st *me = ptr;

    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);
    
    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(ptr);
   // return 0;

}