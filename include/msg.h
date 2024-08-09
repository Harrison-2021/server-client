#ifndef __MSG_H_
#define __MSG_H_

#include "common.h"

#define MSG_PATHNAME "../msg"
#define MSG_PRO_ID 102 

#define MSG_SZ 64

typedef struct msgbuf{
	long mtype;
	char mtext[MSG_SZ];
}msgbuf_t;

extern int msg_create();// 创建消息队列
extern int msg_send(int msgid,long mtype,char *buf);
extern int msg_recv(int msgid,long mtype,char *buf,int len);
extern int msg_del(int msgid);
#endif
