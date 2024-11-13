#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 1024
int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret;
    int pos;
    // 命令行参数小于2报错退出
    if (argc < 2)
    {
        fprintf(stderr, "Usage....\n");
        exit(1);
    }
    // 打开命令行第二个参数传入的文件名以只读的方式打开文件
    sfd = open(argv[1], O_RDONLY);
    if (sfd < 0)
    {
        perror("open_sfd()");
        exit(1);
    }

    while (1)
    {
        // 读取命令行参数传入的文件读取1024个字节读取到buf中
        len = read(sfd, buf, BUFSIZE);
        if (len < 0)
        {
            perror("read()");
            break;
        }
        // 没有读取到退出循环
        if (len == 0)
            break;
        pos = 0;
        // 读取到数据
        while (len > 0)
        {
            // 读取到数据写入到标准输出当中一次写入读取len个字节
            ret = write(dfd, buf + pos, len);
            if (ret < 0)
            {
                perror("write()");
                close(sfd);
                close(dfd);
                exit(1);
            }
            // 写入的偏移量
            pos += ret;
            // 剩余写入字节长度
            len -= ret;
        }
    }
    // 关闭打开命令行传入参数的文件
    close(sfd);
    close(dfd);
    exit(0);
}
