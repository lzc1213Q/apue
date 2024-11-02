#ifndef PROTO_H__
#define PROTO_H__

#define NAMESIZE        32

/* 通讯双方生成 key 值共同使用的文件 */
#define KEYPATH            "/tmp/out"
/* 通讯双方生成 key 值共同使用的 salt 值 */
#define KEYPROJ            'a'

/* 消息类型，只要是大于 0 的合法整数即可 */
#define MSGTYPE            10

/* 通讯双方约定的协议 */
struct msg_st
{
    long mtype;
    char name[NAMESIZE];
    int math;
    int chinese;
};

#endif