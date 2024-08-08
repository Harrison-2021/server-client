#include "server-client.h"

 /* 定义全部变量，记录环形队列数据结构的地址 */
static p_shm_fifo g_shmfifo = NULL;
/* 线程池 */
static p_tpool g_pool = NULL;

/* 1. 服务器初始化*/
void server_init() {
    g_shmfifo = shm_fifo_init(BLOCK_SZ,sizeof(packet_t));
    g_pool = thread_pool_init(THREAD_CNT,TASK_CNT);
}

/* 2. 服务器数据接收接口设计 */
void runloop() {
    p_packet packet = NULL;

    while(1) {
        //为线程池中每个线程分配独立数据包内存-线程释放后释放此资源
        packet = (p_packet)malloc(sizeof(packet_t));
        if(!packet) {
            perror("[ERROR]: malloc packet ");
            return;
        }
        memset(packet, 0, sizeof(packet_t));

        //从环形队列中获取数据-生产者消费者模型-信号量与互斥锁
        shm_fifo_get(g_shmfifo,packet);
        #ifdef DEBUG
		printf("topic : %s,pid : %d,mode : %d,content : %s\n",
					packet->topic,
					packet->pid,
					packet->mode,
					packet->content);
        #endif
        //将获取的数据添加到线程池中，线程池会调用线程执行传入的任务函数与数据包参数
        thread_pool_add_task(g_pool,task_handler,(void*)packet);
    }
}

/* 3. 数据分发-任务函数 
    @param arg：packet数据包指针*/
void task_handler(void* arg) {
    p_packet packet = (p_packet)arg;
    if(packet->mode == PUBLISH) { // 处理发布消息任务
        printf("[DEBUG]: publish.\n");
    } else if(packet->mode == SUBSCRIBE) { // 处理订阅消息任务
        printf("[DEBUG]: subscribe.\n");
    }
}

int main() {
    // server_init();
    // runloop();
    hashmap_test();
    return 0;
}