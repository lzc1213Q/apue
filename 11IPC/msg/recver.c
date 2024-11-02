#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

int main()
{
    key_t key;
    int msgid;
    struct msg_st rbuf;

    // 通过 /tmp/out 文件和字符 'a' 生成唯一的 key，注意文件必须是真实存在的
    key = ftok(KEYPATH, KEYPROJ);
    if (key < 0)
    {
        perror("ftok()");
        exit(1);
    }

    // 接收端应该先启动，所以消息队列由接收端创建
    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    // 不停的接收消息
    while (1)
    {
        // 没有消息的时候会阻塞等待
        if (msgrcv(msgid, &rbuf, sizeof(rbuf) - sizeof(long), 0, 0) < 0)
        {
            perror("msgrcv");
            exit(1);
        }

        /* 用结构体中强制添加的成员判断消息类型，
         * 当然在这个例子中只有一种消息类型，所以不判断也可以。
         * 如果包含多种消息类型这里可以写一组 switch...case 结构
         */
        if (rbuf.mtype == MSGTYPE)
        {
            printf("Name = %s\n", rbuf.name);
            printf("Math = %d\n", rbuf.math);
            printf("Chinese = %d\n", rbuf.chinese);
        }
    }

    /* 谁创建谁销毁。
     * 当然这个程序是无法正常结束的，只能通过信号杀死。
     * 使用信号杀死之后大家可以用 ipcs(1) 命令查看一下，消息队列应该是没有被销毁的，
     * 大家可以使用上面我们提到的 ipcrm(1) 命令把它删掉。
     */
    msgctl(msgid, IPC_RMID, NULL);

    exit(0);
}
