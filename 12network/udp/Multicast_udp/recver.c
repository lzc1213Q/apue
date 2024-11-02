// recever.c
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

	size = sizeof(struct msg_st) + NAMEMAX - 1; // -uint8_t name[1];
	rbufp = malloc(size);
	if (rbufp == NULL)
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

	// 设置 加入多播组需要的信息
	struct ip_mreqn mreq;
	inet_pton(AF_INET, MGROUP, &mreq.imr_multiaddr);  // 设置多播地址
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address); // 设置当前自己IP地址
	mreq.imr_ifindex = if_nametoindex("eth0");		  // 设置网络索引号

	// 对指定Socket sd 的IPPROTO_IP层面 的IP_ADD_MEMBERSHIP属性进行设置，表示加入一个多播组，mreq参数为加入多播组需要的信息。
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr); // 0.0.0.0:any address

	if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	/* !!!! */
	raddr_len = sizeof(raddr);

	while (1)
	{
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
