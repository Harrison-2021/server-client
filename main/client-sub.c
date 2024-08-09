#include "server-client.h"

 /* 定义全部变量，记录环形队列数据结构的地址 */
static p_shm_fifo g_shmfifo = NULL;
static int g_msgid;

/* 1. 客户端接口初始化*/
void client_sub_init() {
    g_shmfifo = shm_fifo_init(BLOCK_SZ,sizeof(packet_t));
    if(!g_shmfifo) return;
    g_msgid = msg_create();
    if(g_msgid == -1) return;
}

void* do_recv_msg(void* arg) {
    char buffer[CONTENT_SZ] = {0};

    while(1) {
        memset(buffer, 0, sizeof(buffer));
        //  消息队列的消息节点类型为当前进程id
        msg_recv(g_msgid, getpid(), buffer, sizeof(buffer));
        printf("message: < %s >\n",buffer);

    }
    pthread_exit(NULL);
}
/* 2. 客户端订阅的功能 
    @param topic: 主题内容*/
void subscribe(char *topic) {
    packet_t packet;
    int err;
    pthread_t tid;

    // 接收订阅主题推送的消息-创建一个线程一直阻塞等待
    err = pthread_create(&tid, NULL, do_recv_msg, NULL);
    if(err) {
        perror("[ERROR]: pthread_create ");
        return;
    }
    pthread_detach(tid);

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
    while(1){}; // 防止进程结束，子线程释放
    return 0;
}