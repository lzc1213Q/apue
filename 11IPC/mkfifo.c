#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#define PATHNAME "/tmp/myfifo"

int main(void)
{
    pid_t pid;
    int fd = -1;
    char buf[BUFSIZ] = "";

    // 创建一个命名管道，大家可以用 ls -l 命令查看这个管道文件的属性
    if (mkfifo(PATHNAME, 0644) < 0)
    {
        perror("mkfifo()");
        exit(1);
    }

    fflush(NULL);
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if (!pid)
    { // parent
        pid = fork();
        if (pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if (!pid)
        { // parent
            // 两个子进程都创建完之后父进程直接退出，使两个子进程不具有亲缘关系。
            exit(0);
        }
        /* child 2 */
        /* 像操作普通文件一样对这个管道进行 open(2)、read(2)、write(2)、close(2) */
        fd = open(PATHNAME, O_RDWR);
        if (fd < 0)
        {
            perror("open()");
            exit(1);
        }
        read(fd, buf, BUFSIZ);
        printf("%s", buf);
        write(fd, " World!", 8);
        close(fd);
        exit(0);
    }
    else
    { // child 1
        fd = open(PATHNAME, O_RDWR);
        if (fd < 0)
        {
            perror("open()");
            exit(1);
        }
        write(fd, "Hello", 6);
        sleep(1); // 刚写完管道不要马上读，等第二个进程读取完并且写入新数据之后再读。
        read(fd, buf, BUFSIZ);
        close(fd);
        puts(buf);
        // 肯定是这个进程最后退出，所以把管道文件删除，不然下次再创建的时候会报文件已存在的错误
        remove(PATHNAME);
        exit(0);
    }

    return 0;
}
