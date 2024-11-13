#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{
  // 定义标准文件io FILE指针
    FILE *fp;
    // 打开文件
    fp = fopen("/tmp/out","r");
    // 文件指针返回为空则打开文件失败
    if(fp == NULL)
    {
      // printf(stderr,"fopen() failed erron = %d\n",errno);
      //  perror("fopen()");//perror 关联errno报错信息
        // 文件打开失败向标准错误输出报错
        fprintf(stderr,"fopen():%s\n",strerror(errno));
        //fopen():No such file or directory
        // 退出进程
        exit(1);
    }
    // 关闭文件
    fclose(fp);
    puts("OK!");
    exit(0);
    
}