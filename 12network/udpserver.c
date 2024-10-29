//socket udp 服务端
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
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
    //创建socket对象
    int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
 
    //创建网络通信对象
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9635);
    //server_addr.sin_addr.s_addr = INADDR_ANY; //绑定在 0.0.0.0 地址
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//绑定在 127.0.0.1 环回地址
 
    //绑定socket对象与通信链接
    int ret = bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret < 0)
    {
        printf("bind fail\n");
        close(sockfd);
        return -1;
    }
 
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
 
    MSG_INTF_S *pstRecvMsg = NULL;
    MSG_INTF_S stSendMsg;
 
    pstRecvMsg = (MSG_INTF_S *)malloc(10240);
 
    while(1)
    {
        memset(pstRecvMsg, 0, 10240);
        recvfrom(sockfd, pstRecvMsg, 10240, 0, (struct sockaddr*)&client_addr, &len);
 
        printf("server: recv a connect: ip:%s, port:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
 
        printf("server: recv szMagicNum[0]  = 0x%X\n", pstRecvMsg->szMagicNum[0]);
        printf("server: recv szMagicNum[1]  = 0x%X\n", pstRecvMsg->szMagicNum[1]);
        printf("server: recv szMagicNum[2]  = 0x%X\n", pstRecvMsg->szMagicNum[2]);
        printf("server: recv szMagicNum[3]  = 0x%X\n", pstRecvMsg->szMagicNum[3]);
        printf("server: recv u32MsgIdx      = %d\n", pstRecvMsg->u32MsgIdx);
        printf("server: recv u16MsgParamLen = %d\n", pstRecvMsg->u16MsgParamLen);
        printf("server: recv pMsgParam      = %s\n", pstRecvMsg->pMsgParam);
 
        ///
 
        // 发一个消息给客户端
        memset(&stSendMsg, 0, sizeof(MSG_INTF_S));
        stSendMsg.szMagicNum[0] = 0x11;
        stSendMsg.szMagicNum[1] = 0x22;
        stSendMsg.szMagicNum[2] = 0x33;
        stSendMsg.szMagicNum[3] = 0x44;
 
        sendto(sockfd, &stSendMsg, sizeof(MSG_INTF_S), 0, (struct sockaddr*)&client_addr, len);
    }
 
    close(sockfd);
    return 0;
}