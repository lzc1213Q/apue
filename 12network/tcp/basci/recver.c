// server.c
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "proto.h"

#define IPSTRSIZE 40
#define BUFSIZE 1024

static void server_job(int sd)
{
	char buf[BUFSIZE];
	int len;

	// 将时间戳 以 FMT_STAMP格式 存储到 buf中
	len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

	if (send(sd, buf, len, 0) < 0)
	{
		perror("send()");
		exit(1);
	}
}

int main()
{
	int sd, newsd;

	struct sockaddr_in laddr, raddr;
	socklen_t raddr_len;
	char ipstr[IPSTRSIZE];

	// 取得SOCKET, 用 AF_INET协议族中 默认支持流式套接字的协议(IPPROTO_TCP) 来完成流式套接字传输，
	sd = socket(AF_INET, SOCK_STREAM, 0 /*IPPROTO_TCP*/);
	if (sd < 0)
	{
		perror("soccket()");
		exit(1);
	}

	// 对指定Socket sd 的SOL_SOCKET层面 的 SO_REUSEADDR属性进行设置，即打开该属性，如果发现 端口没有释放，该属性会马上释放该端口，并且让我们绑定成功。
	int val = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	// 设置 AF_INET 协议族地址信息结构体，AF_INET 协议族中的 协议地址类型为 struct sockaddr_in

	laddr.sin_family = AF_INET; // 协议族为 AF_INET
	// 设置端口为1989，因为需要将自己的地址信息(包括端口信息)发出去，所以需要注意字节序问题，即 从主机发向网络，htons
	laddr.sin_port = htons(atoi(SERVERPORT));
	// 设置IP
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr); // 0.0.0.0:any address

	// 给SOCKET绑定一个地址，关联到目标协议族地址信息结构体，即绑定IP，端口
	//(void *)&laddr, 实际并没有struct sockaddr类型，所以暂时强转为 void *
	if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	// 将socket置为监听模式, 承受C端最大数量为200
	if (listen(sd, 200) < 0)
	{
		perror("listen");
		exit(1);
	}

	/* !!!! */
	raddr_len = sizeof(raddr);

	while (1)
	{
		// 接受连接,保存对端地址信息，如果成功，将返回一个非负整数，它是所接受套接字的描述符，即newsd
		// 默认为阻塞等待连接
		newsd = accept(sd, (void *)&raddr, &raddr_len);
		if (newsd < 0)
		{
			perror("accept");
			exit(1);
		}

		// 将接收到的对端地址中IP信息 从大整数 转换为 点分式 保存到ipstr数组
		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);

		// 打印对端 IP:端口
		// raddr.sin_port  是从socket接收到的信息，不是单字节信息，需要字节序转换 ntohs
		printf("Client %s:%d\n", ipstr, ntohs(raddr.sin_port));

		server_job(newsd);
		close(newsd);
	}

	close(sd);

	exit(0);
}
