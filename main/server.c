#include "server-client.h"

 /* 定义全部变量，记录环形队列数据结构的地址 */
static p_shm_fifo g_shmfifo = NULL;
/* 线程池 */
static p_tpool g_pool = NULL;
/* hashmap */
static hashtable_t** g_hashtable = NULL;
/* 消息队列 */
static int g_msgid;

/* 1. 服务器初始化*/
void server_init() {
    g_shmfifo = shm_fifo_init(BLOCK_SZ,sizeof(packet_t));
    g_pool = thread_pool_init(THREAD_CNT,TASK_CNT);
    g_hashtable = create_hashtable();
    g_msgid = msg_create();
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
		printf("getfrom shmfifo : topic : %s,pid : %d,mode : %d,content : %s\n",
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
        //printf("[DEBUG]: publish.\n");
        do_publish(packet->topic,packet->pid,packet->content);
    } else if(packet->mode == SUBSCRIBE) { // 处理订阅消息任务
        // printf("[DEBUG]: subscribe.\n");
        do_subscribe(packet->topic,packet->pid);
    }
}

/* 4. 订阅消息处理函数接口 */
void do_subscribe(char *topic,pid_t pid) {
    data_t value;
    strcpy(value.topic, topic);
    value.pid = pid;

    // 将数据存储到 hashmap 中-向hash表注册
    insert_data_hash(g_hashtable,topic,value);

    #ifdef DEBUG
    printf_hash_table(g_hashtable); // 输出 hashmap 
    #endif
}

/* 5. 发布消息函数接口 */
void do_publish(char *topic,pid_t pid,char *content) {
    pid_t result[RESULT_SZ];
    int ret;
    // 1.发布消息后（放入共享内存），通过hash查找订阅该主题的客户的id
    ret = search_hash_table(g_hashtable,topic,result);
    #if 0
    printf("ret = %d\n",ret);
    #endif
    // 2.将分布的内容通过消息队列通知所有订阅的客户端id
    for(int i = 0; i < ret; i++) {
        msg_send(g_msgid, result[i],content);
    }
}

int main() {
    server_init();
    runloop();
    
    return 0;
}