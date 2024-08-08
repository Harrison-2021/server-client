#include "server-client.h"

 /* 定义全部变量，记录环形队列数据结构的地址 */
static p_shm_fifo g_shmfifo = NULL;

/* 1. 客户端接口初始化*/
void client_sub_init() {
    g_shmfifo = shm_fifo_init(BLOCK_SZ,sizeof(packet_t));
}

/* 2. 客户端订阅的数据发送功能 
    @param topic: 主题内容*/
void subscribe(char *topic) {
    packet_t packet;
    // 封装订阅数据包
    strcpy(packet.topic, topic);
    packet.pid = getpid();
    packet.mode = SUBSCRIBE;

    // 通过环形队列发送给服务器
    shm_fifo_put(g_shmfifo,&packet);
}

int main() {
    client_sub_init();
    subscribe("hello");
    while(1){};
    return 0;
}