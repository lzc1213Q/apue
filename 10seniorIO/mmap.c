#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FNAME "/etc/services"

int main(void)
{
    int fd, i;
    char *str;
    struct stat statres;
    int count = 0;

    fd = open(FNAME, O_RDONLY);
    if (fd < 0)
    {
        perror("open()");
        exit(1);
    }

    // 通过 stat(2) 获得文件大小
    if (fstat(fd, &statres) < 0)
    {
        perror("fstat()");
        exit(1);
    }

    str = mmap(NULL, statres.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (str == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    // 将文件映射到内存之后文件描述符就可以关闭了，直接访问映射的内存就相当于访问文件了。
    close(fd);

    for (i = 0; i < statres.st_size; i++)
    {
        // 因为访问的是文本文件，所以可以把映射的内存看作是一个大字符串处理
        if (str[i] == 'a')
        {
            count++;
        }
    }

    printf("count = %d\n", count);

    // 用完了别忘了解除映射，不然会造成内存泄漏！
    munmap(str, statres.st_size);

    exit(0);
}
