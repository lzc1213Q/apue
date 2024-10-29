#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile int loop = 1;
static void alarm_handler(int s)
{
    loop = 0;
}
int main()
{
    time_t end;
    int64_t count = 0;
    alarm(5);
    signal(SIGALRM, alarm_handler);
    end = time(NULL)+5;

    while(loop)
    count ++;

    printf("%lld \n",count);
    
    exit(0);

} 