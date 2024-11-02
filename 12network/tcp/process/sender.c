#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "proto.h"

int main(int argc, char *argv[])
{
	int sd;
	struct sockaddr_in raddr;
	long long stamp;
	FILE *fp;

	if (argc < 2)
	{
		fprintf(stderr, "Usage...!\n");
		exit(1);
	}

	// 取得SOCKET, 用 AF_INET协议族中 默认支持流式套接字的协议(IPPROTO_TCP) 来完成流式套接字传输，
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
	{
		perror("soccket()");
		exit(1);
	}

	// bind()

	// 设置对端地址信息，即设置S端地址信息
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);

	// 建立连接
	if (connect(sd, (void *)&raddr, sizeof(raddr)) < 0)
	{
		perror("connect()");
		exit(1);
	}

	/*
	接下来就是接收数据，正常使用 recv()函数接收，这里用文件操作来解析接收的数据来加深 一切皆文件的思想，本身sd就是一个文件描述符
	将给定文件描述符 并指定操作权限，转换为 FILE* 文件流的操作，转换成功之后，我们对当前socket的操作就完全转换为了标准IO 的操作,
	这样所有可移植的标准库函数都可以使用，fread()  fwrite() ...
	*/
	fp = fdopen(sd, "r+");
	if (fp == NULL)
	{
		perror("fdopen()");
		exit(1);
	}

	// 从指定流fp中拿数据 以 format格式存储到 目标地址
	if (fscanf(fp, FMT_STAMP, &stamp) < 1)
		fprintf(stderr, "Bad format!\n");
	else
		fprintf(stdout, "stamp = %lld\n", stamp);

	// 关闭fp
	fclose(fp);

	exit(0);
}
