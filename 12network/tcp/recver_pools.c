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
#define PROCNUM 4

static void server_loop(int sd);

static void server_job(int sd)
{
	char buf[BUFSIZE];
	int len;

	len = sprintf(buf, FMT_STAMP, (long long)time(NULL));

	if (send(sd, buf, len, 0) < 0)
	{
		perror("send()");
		exit(1);
	}
}

int main()
{
	int sd;
	pid_t pid;
	struct sockaddr_in laddr;

	sd = socket(AF_INET, SOCK_STREAM, 0 /*IPPROTO_TCP*/);
	if (sd < 0)
	{
		perror("soccket()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr); // 0.0.0.0:any address

	if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	if (listen(sd, 200) < 0)
	{
		perror("listen");
		exit(1);
	}

	for (int i = 0; i < PROCNUM; i++)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if (pid == 0)
		{
			server_loop(sd);
			exit(0);
		}
	}
	// 只有父进程会执行这段代码，等待所有子进程
	for (int i = 0; i < PROCNUM; i++)
		wait();
	// 关闭监听套接字
	close(sd);
	return 0;
}

static void server_loop(int sd)
{
	struct sockaddr_in raddr;
	socklen_t raddr_len;
	int newsd;
	char ipstr[IPSTRSIZE];

	/* !!!! */
	raddr_len = sizeof(raddr);

	while (1)
	{
		newsd = accept(sd, (void *)&raddr, &raddr_len);
		if (newsd < 0)
		{
			perror("accept");
			continue; // 继续等待下一个连接
		}

		inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);

		printf("[%d]Client %s:%d\n", getpid(), ipstr, ntohs(raddr.sin_port));

		server_job(newsd);

		close(newsd);
	}

	close(sd);
}
