//sender.c
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "proto.h"

int main(int argc,char *argv[])
{
	int sd;
	struct sockaddr_in raddr;
	struct msg_st *sbufp;
	int size;

	if(argc < 3)
	{
		fprintf(stderr,"Usage ...\n");
		exit(1);
	}

//需要发送的数据包大小，即柔型数组大小
	size = sizeof(struct msg_st) + strlen(argv[2]);
//根据需要发送数据包大小 申请空间
	sbufp = malloc(size);
	if(sbufp == NULL)
	{
		perror("malloc()");
		exit(1);
	}
//创建Socket, 即确定传输协议族，传输协议，传输方式
	sd = socket(AF_INET,SOCK_DGRAM,0);//IPPROTO_UDP
	if(sd < 0)
	{
		perror("soccket()");
		exit(1);
	}

//对指定Socket sd 的SOL_SOCKET层面 的 SO_BROADCAST属性进行设置，即打开该属性，表示允许将数据发送至广播地址，即打开广播属性。
	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST,&val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}
	//bind() 可省略绑定到本端地址，系统会自动分配
	
// 拷贝命令行中的 名字到 数据包对应位置
	strcpy(sbufp->name,argv[2]);
	sbufp->math = htonl(rand()%100);
	sbufp->chinese = htonl(rand()%100);

//设置地址信息， struct sockaddr_in结构体信息：协议族类型，端口，IP
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	//地址为广播地址，255.255.255.255 广播地址
	inet_pton(AF_INET,"255.255.255.255",&raddr.sin_addr);

	//向 服务端 发送数据
	if(sendto(sd,sbufp,size,0,(void *)&raddr,sizeof(raddr)) < 0)
	{
		perror("sendto()");
		exit(1);
	}

	puts("OK");
	close(sd);
	exit(0);
}
