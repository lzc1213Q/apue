#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
    time_t end;
    int64_t count = 0;
    // 5s时间内count疯狂自加
    end = time(NULL) + 5;

    while (time(NULL) <= end)
        count++;

    printf("%lld \n", count);

    exit(0);
}