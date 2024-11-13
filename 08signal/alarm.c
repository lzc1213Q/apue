#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    alarm(10);
    alarm(5);
    alarm(1);
    // 响应最后一个alam定时器
    while (1);

    exit(0);
}