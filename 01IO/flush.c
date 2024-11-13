#include <stdlib.h>
#include <stdio.h>
int main()
{
    //缓冲区的作用：大多数情况下是好事合并系统调用
    // 行缓冲:换行时候刷新，满了的时候刷新，强制刷新（标准输出就是这样的，因为是终端设备）
    // 全缓冲：满了的时候刷新，强制刷新（默认，只要不是终端设备）
    // 无缓冲：如stderr，需要立即输出的内容
    // 修改缓冲区模式： setvbuf

    int i;
    printf("Berfor while()");
    // 刷新标准输出流
    fflush(stdout);

    while(1);

    printf("After while()");
    fflush(stdout);

    exit(0);
}