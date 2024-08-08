#ifndef _SERVER_CLIENT_H
#define _SERVER_CLIENT_H

#include "common.h"
#include "shm-fifo.h"
#include "thread-pool.h"
#include "hashmap.h"

#define TOPIC_SZ    64
#define CONTENT_SZ  64

#define BLOCK_SZ    10

#define THREAD_CNT  10
#define TASK_CNT    10

// 工作模式
enum work_mode {
    PUBLISH = 0,    // 发布消息
    SUBSCRIBE,      // 订阅消息
};

// 传输的数据包
typedef struct packet{
    char topic[TOPIC_SZ];   //主题内容
    pid_t pid;              // 发布与订阅进程 ID-客户端进程
    enum work_mode mode;    // 工作模式
    char content[CONTENT_SZ];// 消息内容
}packet_t, *p_packet;

// 客户端订阅信息功能
extern void client_sub_init();
extern void subscribe(char *topic);

// 服务端接口
extern void server_init();
extern void runloop();
extern void task_handler(void *arg);

#endif