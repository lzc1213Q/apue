#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{
    FILE *fp = NULL;
    int count = 0;
    fp = fopen("tmp","r");
    while(1)
    {
        if(fp == NULL)
        {
            
            perror("fopen()");//perror 关联errno报错信息
            break;
        }
        count++;
    printf("count %d\n",count);
    }
    //fclose(fp);
    puts("OK!");
    exit(0);
    
}
