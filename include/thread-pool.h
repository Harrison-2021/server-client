#ifndef __THREAD_H_
#define __THREAD_H_

#include "common.h"


typedef struct tpool_task{
	void (*function)(void *arg);// 指向任务函数的指针,并传入参数-一般为指向线程池的指针
	void *arg;
}tpool_task_t, *p_tpool_task;

typedef struct tpool{

    tpool_task_t *tp_task; // 指向任务队列空间的指针-任务中的每个节点

    // 维护的任务队列
    int tp_qcapacity;//任务队列容量
    int tp_qsize;// 当前任务数量

    int tp_qfront;// 任务队列头
    int tp_qrear;// 队列尾

    // 维护的线程集合
    pthread_t *tp_work_thread_ids;//存储工作线程id空间的指针
    int tp_number_of_threads;// 总的线程数量

    // 线程池的访问控制
    pthread_mutex_t tp_mutex_pool;//线程池互斥锁
    pthread_cond_t tp_cond_empty; // 条件变量 : 任务队列是否为空
    pthread_cond_t tp_cond_full;//  条件变量 : 任务队列是否为满

    bool tp_shutdown; // 销毁线程池的标志

}tpool_t, *p_tpool;

extern tpool_t *thread_pool_init(int cnt,int queuesize);
extern void thread_pool_add_task(tpool_t *pool,void (*task)(void *arg),void *arg);
extern int thread_pool_destroy(tpool_t *pool);
void *tp_worker(void *arg);

#endif
