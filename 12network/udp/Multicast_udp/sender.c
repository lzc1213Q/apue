// sender.c
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "proto.h"

int main(int argc, char *argv[])
{
	int sd;
	struct sockaddr_in raddr;
	struct msg_st *sbufp;
	int size;

	if (strlen(argv[2]) > NAMEMAX)
	{
		fprintf(stderr, "Name is too long!\n");
		exit(1);
	}

	size = sizeof(struct msg_st) + strlen(argv[2]);
	sbufp = malloc(size);
	if (sbufp == NULL)
	{
		perror("malloc()");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0); // IPPROTO_UDP
	if (sd < 0)
	{
		perror("soccket()");
		exit(1);
	}

	// 设置 创建多播组需要的信息
	struct ip_mreqn mreq;
	inet_pton(AF_INET, MGROUP, &mreq.imr_multiaddr);  // 设置多播地址
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address); // 设置当前自己IP地址
	mreq.imr_ifindex = if_nametoindex("eth0");		  // 设置网络设备索引号 指定所用网卡名

	// 对指定Socket sd 的IPPROTO_IP层面 的IP_MULTICAST_IF属性进行设置，表示创建一个多播组。mreq参数为创建多播组需要的信息。
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	// bind()

	strcpy(sbufp->name, argv[2]);
	sbufp->math = htonl(rand() % 100);
	sbufp->chinese = htonl(rand() % 100);

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, MGROUP, &raddr.sin_addr); // 对端地址为多播组地址

	if (sendto(sd, sbufp, size, 0, (void *)&raddr, sizeof(raddr)) < 0)
	{
		perror("sendto()");
		exit(1);
	}

	puts("OK");

	close(sd);

	exit(0);
}
