#include "msg.h"

/* 1. 创建消息队列
    @return msgid*/
int msg_create() {
    int msgid;

    key_t key = ftok(MSG_PATHNAME, MSG_PRO_ID);
    // 先判断是否创建过，创建过，无需重复创建
    msgid = msgget(key, 0);
    if(msgid == -1) {
        msgid = msgget(key, IPC_CREAT | 0644);
        if(msgid == -1) {
            perror("[ERROR]: megget ");
            return -1;
        }
    }

    return msgid;
}

/* 2. 消息队列发送
    @param mtype: 消息节点类型编号
    @param buf  : 消息内容*/
int msg_send(int msgid,long mtype,char *buf) {
    int ret;
    msgbuf_t msg;
    msg.mtype = mtype;
    strcpy(msg.mtext, buf);

    //发送消息-多发一个字节'\0',如果用sizeof(msg.mtext),就是整个MSG_SZ=64个字节
    ret = msgsnd(msgid,(const void*)&msg, strlen(msg.mtext)+1,0);
    if(ret == -1) {
        perror("[ERROR]: msgsnd ");
        return -1;
    }
    return ret;
}

/* 3. 消息队列接收
    @param buf: message
    @param len: 消息内容的长度*/
int msg_recv(int msgid,long mtype,char *buf,int len) {
    msgbuf_t msg;
    int ret;

    ret = msgrcv(msgid,(void*)&msg,len,mtype,0);
    if (ret == -1){
        perror("[ERROR] msgrecv() ");
        return -1;
    }

    strcpy(buf,msg.mtext);
    return ret;
}

/* 4. 消息队列删除 */
int msg_del(int msgid) {
    return msgctl(msgid, IPC_RMID, NULL);
}
