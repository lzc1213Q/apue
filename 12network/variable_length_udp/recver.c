// revever.c
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "proto.h"

int main()
{
    int sd;
    struct sockaddr_in laddr, raddr;
    struct msg_st *rbufp;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];
    int size;

    // 根据服务端 即将接受数据大小 申请空间 用于存储接收数据
    size = sizeof(struct msg_st) + NAMEMAX - 1; // -uint8_t name[1];
    rbufp = malloc(size);
    if (rbufp == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    // 创建Socket, 即确定传输协议族，传输协议，传输方式
    sd = socket(AF_INET, SOCK_DGRAM, 0); // IPPROTO_UDP
    if (sd < 0)
    {
        perror("soccket()");
        exit(1);
    }

    // 设置 本端，即服务端 地址信息， struct sockaddr_in结构体信息：协议族类型，端口，IP
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr); // 0.0.0.0:any address

    // 绑定 Socket 到 本端地址
    if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    /* !!!! */
    raddr_len = sizeof(raddr);

    while (1)
    {
        // 接受数据，数据存储于rbufp，将对端地址信息(struct sockaddr_in)存储于raddr
        recvfrom(sd, rbufp, size, 0, (void *)&raddr, &raddr_len);

        inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);

        printf("---MESSAGE FROM %s:%d---\n", ipstr, ntohs(raddr.sin_port));

        printf("NAME = %s\n", rbufp->name);
        printf("MATH = %d\n", ntohl(rbufp->math));
        printf("CHINESE = %d\n", ntohl(rbufp->chinese));
    }

    close(sd);

    exit(0);
}
