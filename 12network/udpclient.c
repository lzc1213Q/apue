//socket udp 客户端
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct
{
    unsigned char szMagicNum[4];
    unsigned int u32MsgIdx;
    unsigned int u16MsgParamLen;
    unsigned char pMsgParam[0]; // 表示不定长
} MSG_INTF_S;
 
// 使用不定长的结构体发送数据的关键在于:
// 不能在结构体中使用指针变量, 改用pMsgParam[0]这种写法
// 因为pMsgParam[0]不定长, 所以结构体变量要分配到堆中, 即用malloc或new来分配空间
 
// 接收的话, 就申请一个大一点的buffer, 一次接收完发送端发过来的不定长数据即可
// 因为UDP的特点: UDP发一包就是一包, 要么收到, 要么收不到一个完整的包
 
int main(void)
{
    int buf = 0;
    unsigned int cnt = 0;
 
    //创建socket对象
    int sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
 
    //绑定客户端地址信息
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(9633);
    //client_addr.sin_addr.s_addr = INADDR_ANY; //绑定在 0.0.0.0 地址
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//绑定在 127.0.0.1 环回地址
 
    if(bind(sockfd, (struct sockaddr* )&client_addr, sizeof(struct sockaddr_in)) < 0)
    {
        printf("bind failed\n");
        return -1;
    }
 
    //创建网络通信对象
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9635);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t len = sizeof(server_addr);
 
    int TotalDataLen = 0;
    MSG_INTF_S *pstSendMsg = NULL;
    MSG_INTF_S stRecvMsg;
 
    while(1)
    {
        printf("client: input a number: ");
        scanf("%d", &buf);
 
        char *msg = (char *)"Too young too simple!!!"; // 准备不定长的数据
        TotalDataLen = sizeof(MSG_INTF_S) + strlen(msg) + 1;
        pstSendMsg = (MSG_INTF_S *)malloc(TotalDataLen); // 开辟一块空间
        pstSendMsg->szMagicNum[0] = 0x55;
        pstSendMsg->szMagicNum[1] = 0x66;
        pstSendMsg->szMagicNum[2] = 0x77;
        pstSendMsg->szMagicNum[3] = 0x88;
        pstSendMsg->u32MsgIdx = cnt++;
        pstSendMsg->u16MsgParamLen = strlen(msg) + 1;
        memcpy(pstSendMsg->pMsgParam, msg, pstSendMsg->u16MsgParamLen);
 
        sendto(sockfd, pstSendMsg, TotalDataLen, 0, (struct sockaddr*)&server_addr, len);
 
        if(pstSendMsg)
        {
            free(pstSendMsg);
            pstSendMsg = NULL;
        }
 
        
 
        // 接收服务端返回来的消息, 以此确认服务端已成功收到数据
        memset(&stRecvMsg, 0, sizeof(MSG_INTF_S));
        recvfrom(sockfd, &stRecvMsg, sizeof(MSG_INTF_S), 0, (struct sockaddr*)&server_addr, &len);
        printf("client: recv a connect: ip:%s, port:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
 
        if(stRecvMsg.szMagicNum[0] == 0x11 && stRecvMsg.szMagicNum[1] == 0x22
            && stRecvMsg.szMagicNum[2] == 0x33 && stRecvMsg.szMagicNum[3] == 0x44)
        {
            printf("client: server receive success\n\n");
        }
        else
        {
            printf("client: server reveive fail\n\n");
        }
    }
 
    close(sockfd);
    return 0;
}