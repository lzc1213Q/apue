#ifndef PROTO_H_
#define PROTO_H_

#define RCVPORT "1989"
#define NAMEMAX 512-8-8 // math+chinese 8 + UDP报头 8
#define IPSTRSIZE 40
struct msg_st
{
	uint32_t math;
	uint32_t chinese;
	uint8_t name[1];
}__attribute__((pack));
#endif
