
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 1024
#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"

/* 状态机的各种状态 */
enum
{
    STATE_R = 1,
    STATE_W,
    STATE_AUTO, // 添加这个值是为了起到分水岭的作用，小于这个值的时候才需要使用 select(2) 监视
    STATE_Ex,
    STATE_T
};
static int max(int a, int b)
{
    if (a < b)
        return b;
    return a;
}

/* 状态机，根据不同的需求设计不同的成员 */
struct fsm_st
{
    int state;         // 状态机当前的状态
    int sfd;           // 读取的来源文件描述符
    int dfd;           // 写入的目标文件描述符
    char buf[BUFSIZE]; // 缓冲
    int len;           // 一次读取到的实际数据量
    int pos;           // buf 的偏移量，用于记录坚持写够 n 个字节时每次循环写到了哪里
    char *errstr;      // 错误消息
};

/* 状态机驱动 */
static void fsm_driver(struct fsm_st *fsm)
{
    int ret;

    switch (fsm->state)
    {
    case STATE_R: // 读态
        fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
        if (fsm->len == 0) // 读到了文件末尾，将状态机推向 T态
            fsm->state = STATE_T;
        else if (fsm->len < 0) // 读取出现异常
        {
            if (errno == EAGAIN) // 如果是假错就推到 读态，重新读一次
                fsm->state = STATE_R;
            else // 如果是真错就推到 异常态
            {
                fsm->errstr = "read()";
                fsm->state = STATE_Ex;
            }
        }
        else // 成功读取到了数据，将状态机推到 写态
        {
            fsm->pos = 0;
            fsm->state = STATE_W;
        }
        break;

    case STATE_W: // 写态
        ret = write(fsm->dfd, fsm->buf + fsm->pos, fsm->len);
        if (ret < 0) // 写入出现异常
        {
            if (errno == EAGAIN) // 如果是假错就再次推到 写态，重新再写入一次
                fsm->state = STATE_W;
            else // 如果是真错就推到 异常态
            {
                fsm->errstr = "write()";
                fsm->state = STATE_Ex;
            }
        }
        else // 成功写入了数据
        {
            fsm->pos += ret;
            fsm->len -= ret;
            if (fsm->len == 0) // 如果将读到的数据完全写出去了就将状态机推向 读态，开始下一轮读取
                fsm->state = STATE_R;
            else // 如果没有将读到的数据完全写出去，那么状态机依然推到 写态，下次继续写入没写完的数据，实现“坚持写够 n 个字节”
                fsm->state = STATE_W;
        }

        break;

    case STATE_Ex: // 异常态，打印异常并将状态机推到 T态
        perror(fsm->errstr);
        fsm->state = STATE_T;
        break;

    case STATE_T: // 结束态，在这个例子中结束态没有什么需要做的事情，所以空着
        /*do sth */
        break;
    default: // 程序很可能发生了溢出等不可预料的情况，为了避免异常扩大直接自杀
        abort();
    }
}

static void relay(int fd1, int fd2)
{
    int fd1_save, fd2_save;
    struct fsm_st fsm12, fsm21;
    fd_set rset, wset; // 读写文件描述符集合

    fd1_save = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, fd1_save | O_NONBLOCK);
    fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd2_save | O_NONBLOCK);

    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    while (fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        // 布置监视任务
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        // 读态监视输入文件描述符；写态监视输出文件描述符
        if (fsm12.state == STATE_R)
            FD_SET(fsm12.sfd, &rset);
        if (fsm12.state == STATE_W)
            FD_SET(fsm12.dfd, &wset);
        if (fsm21.state == STATE_R)
            FD_SET(fsm21.sfd, &rset);
        if (fsm21.state == STATE_W)
            FD_SET(fsm21.dfd, &wset);

        if (fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
        {
            // 以阻塞形式监视
            if (select(max(fd1, fd2) + 1, &rset, &wset, NULL, NULL) < 0)
            {
                if (errno == EINTR)
                    continue;
                perror("select()");
                exit(1);
            }
        }

        // 查看监视结果
        if (FD_ISSET(fd1, &rset) || FD_ISSET(fd2, &wset) || fsm12.state > STATE_AUTO)
            fsm_driver(&fsm12);
        if (FD_ISSET(fd2, &rset) || FD_ISSET(fd1, &wset) || fsm21.state > STATE_AUTO)
            fsm_driver(&fsm21);
    }

    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);
}

int main()
{
    int fd1, fd2;

    // 假设这里忘记将设备 tty1 以非阻塞的形式打开也没关系，因为推动状态机之前会重新设定文件描述符为非阻塞形式
    fd1 = open(TTY1, O_RDWR);
    if (fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd1, "TTY1\n", 5);

    fd2 = open(TTY2, O_RDWR | O_NONBLOCK);
    if (fd2 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd2, "TTY2\n", 5);

    relay(fd1, fd2);

    close(fd1);
    close(fd2);

    exit(0);
}