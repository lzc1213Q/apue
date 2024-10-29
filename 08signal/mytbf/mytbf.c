#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include "mytbf.h"
typedef void (*sighandler_t) (int);
static struct mytbf_st* job[MYTBF_MAX];
static int inited = 0;
//static sighandler_t alrm_handler_save;
static struct sigaction alarm_sa_save;
struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
};


static int get_free_pos(void)
{
    int i;
    for(i = 0; i < MYTBF_MAX; i++ )
    {
        if(job[i] == NULL)
        return i;
    }
    return -1;
}
static void alrm_action(int s, siginfo_t *infop, void *unused)
{
    int i;
    
   // alarm(1);
   if(infop->si_code != SI_KERNEL)
    return ;
   
    for(i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL)
        {
            job[i]->token +=job[i]->cps;
            if(job[i]->token > job[i]->burst)
            {
                job[i]->token = job[i]->burst; 
            }
        }
    }  
}
static void module_unload(void)//释放令牌桶
{
    int i;
    struct itimerval itv;
    // signal(SIGALRM,alrm_handler_save);
    // alarm(0);
    sigaction(SIGALRM,&alarm_sa_save,NULL);
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL,&itv,NULL);
    for(i = 0; i < MYTBF_MAX; i++)
    {
        free(job[i]);
    }
}
static void module_load(void)//开启令牌桶
{
    struct sigaction sa;
    struct itimerval itv;
    sa.sa_sigaction = alrm_action;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =  SA_SIGINFO;
    sigaction(SIGALRM,&sa,&alarm_sa_save);
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL,&itv,NULL);
    // alrm_handler_save = signal(SIGALRM,alrm_handler);
    // alarm(1);
    atexit(module_unload);
}


mytbf_t *mytbf_init(int cps,int burst)
{
    struct mytbf_st *me;
    int pos;
    if(!inited)
    {
        module_load();
        
        inited = 1;
    }
    
    pos = get_free_pos();
    if(pos < 0)
        return NULL;

    me = malloc(sizeof(*me));
    if(me == NULL)
        return NULL;
    
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;
    job[pos] = me;
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
    
    while(me->token <= 0)
        pause();
    n = min(me->token,size);

    me->token -= n;
    return n;

    
}

int mytbf_returntoken(mytbf_t *ptr,int size)
{
    struct mytbf_st *me = ptr;

    if(size <= 0)
        return -EINVAL;
    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;
    
    return size;
    
}
int mytbf_destory(mytbf_t *ptr)
{
    struct mytbf_st *me = ptr;
    job[me->pos] == NULL;
    free(ptr);
    return 0;

}