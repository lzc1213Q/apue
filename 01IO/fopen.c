#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{
    FILE *fp;
    fp = fopen("/tmp/out","r");
    if(fp == NULL)
    {
      // printf(stderr,"fopen() failed erron = %d\n",errno);
      //  perror("fopen()");//perror 关联errno报错信息

        fprintf(stderr,"fopen():%s\n",strerror(errno));
        //fopen():No such file or directory
        exit(1);
    }
   
    fclose(fp);
    puts("OK!");
    exit(0);
    
}