#include "server-client.h"

 /* 定义全部变量，记录环形队列数据结构的地址 */
static p_shm_fifo g_shmfifo = NULL;

/* 1. 客户端接口初始化*/
void client_pub_init() {
    g_shmfifo = shm_fifo_init(BLOCK_SZ,sizeof(packet_t));
}

/* 2. 客户端发布消息 */
void publish(char *topic,const char *content) {
    packet_t packet;

    // 封装数据包
    strcpy(packet.topic, topic);
    strcpy(packet.content, content);
    packet.pid = getpid();
    packet.mode = PUBLISH;

    shm_fifo_put(g_shmfifo, &packet);
}


int main() {
    client_pub_init();
	publish("hello","ABCDE");
    return 0;
}