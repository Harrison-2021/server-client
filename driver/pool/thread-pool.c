#include "thread-pool.h"

/* 1. 线程池的初始化
    @param cnt: 线程数量
    @param queuesize: 环形任务队列大小 */
tpool_t* thread_pool_init(int cnt, int queuesize) {
    int ret;
    // 1.创建线程池结构体对象, 分配空间
    p_tpool pool = (p_tpool)malloc(sizeof(tpool_t));
    if(!pool) {
        perror("[ERROR]: tpool_t malloc ");
        goto err;
    }

    // 2.线程数据初始化
    pool->tp_work_thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * cnt);
    if(!pool->tp_work_thread_ids) {
        perror("[ERROR]: pthread_t malloc ");
        goto err;
    }
    memset(pool->tp_work_thread_ids, 0, sizeof(pthread_t)*cnt);

    pool->tp_number_of_threads = cnt;

    for(int i = 0; i < cnt; i++) {
        ret = pthread_create(&pool->tp_work_thread_ids[i],NULL,tp_worker,pool); // 将线程池结构体传给操作函数
        if(ret) {
            perror("[ERROR]: pthread_create ");
            goto err;
        }
        //分离线程-线程退出时，系统自动释放线程资源
        pthread_detach(pool->tp_work_thread_ids[i]);
    }

    // 3.初始化互斥锁与条件变量
    ret = pthread_mutex_init(&pool->tp_mutex_pool,NULL);
    if(ret) {
        perror("[ERROR]: pthread_mutex_init ");
        goto err;
    }
    ret = pthread_cond_init(&pool->tp_cond_empty, NULL);
    if(ret) {
        perror("[ERROR]: pthread_cond_init empty ");
        goto err;
    }
    ret = pthread_cond_init(&pool->tp_cond_full, NULL);
    if(ret) {
        perror("[ERROR]: pthread_cond_init full ");
        goto err;
    }

    // 4.初始化任务队列数据
    pool->tp_task = (p_tpool_task)malloc(sizeof(tpool_task_t)*queuesize);
    if(!pool->tp_task) {
        perror("[ERROR]: malloc tp_task ");
        goto err;
    }
    memset(pool->tp_task, 0, sizeof(tpool_task_t)*queuesize);

    pool->tp_qcapacity = queuesize;
    pool->tp_qsize = 0;
    pool->tp_qfront = 0;
    pool->tp_qrear = 0;

    // 5.初始化线程池销毁标志
    pool->tp_shutdown = false;

    return pool;

err:
    if(pool && pool->tp_task) {
        free(pool->tp_task);
    }
    if(pool && pool->tp_work_thread_ids) {
        free(pool->tp_work_thread_ids);
    }
    if(pool) {
        free(pool);
    }
    return NULL;

}

/* 2. 线程池添加任务
    @param pool 临界资源-线程池-队列和工作线程
    @param task 任务队列-节点中的任务处理函数入口地址
    @param arg  任务队列-节点中的任务处理函数传递的参数*/
void thread_pool_add_task(p_tpool pool, void (*task)(void *arg),void *arg) {
    // 1.获取线程池互斥锁
    pthread_mutex_lock(&pool->tp_mutex_pool);

    // 2.条件变量-判断任务队列是否已满
    while(pool->tp_qcapacity == pool->tp_qsize && !pool->tp_shutdown) {
        pthread_cond_wait(&pool->tp_cond_full,&pool->tp_mutex_pool);
    }

    // 3.判断销毁标志是否为 true ,释放线程池互斥锁,如果是则直接返回
    if(pool->tp_shutdown) {
        pthread_mutex_unlock(&pool->tp_mutex_pool);
        return;
    }

    // 4.将`任务函数与参数`添加到`任务队列`中,并维护其他队列属性
    pool->tp_task[pool->tp_qrear].function = task;
    pool->tp_task[pool->tp_qrear].arg = arg;
    pool->tp_qrear = (pool->tp_qrear + 1) % pool->tp_qcapacity;
    pool->tp_qsize++;

    // 5.唤醒一个工作线程执行任务
    pthread_cond_signal(&pool->tp_cond_empty);

    // 6.释放线程池互斥锁
    pthread_mutex_unlock(&pool->tp_mutex_pool);

}

/*  3. 线程执行-从任务队列的队首取一个task交给空闲线程执行
    @param arg 创建线程时传递的参数-临界资源*/
void *tp_worker(void *arg) {
    // 1.获取线程池数据结构对象
    p_tpool pool = (p_tpool)arg;
    tpool_task_t task;

    while(1) {
        // 1. 获取线程池的互斥锁
        pthread_mutex_lock(&pool->tp_mutex_pool);
        // 2. 件变量-判断队列是否为空
        while(pool->tp_qsize == 0 && !pool->tp_shutdown) {
            pthread_cond_wait(&pool->tp_cond_empty,&pool->tp_mutex_pool);
        }
        // 3.判断是否销毁
        if(pool->tp_shutdown) {
            pthread_mutex_unlock(&pool->tp_mutex_pool);
            pthread_exit(NULL);
        }
        // 4.从`任务队列中获取一个任务`,从队列头开始获取-先进先出
        //   并维护其他队列属性
        task.function = pool->tp_task[pool->tp_qfront].function;
		task.arg = pool->tp_task[pool->tp_qfront].arg;
		pool->tp_qfront = (pool->tp_qfront + 1) % pool->tp_qcapacity;
		pool->tp_qsize--;

        // 5.唤醒添加任务的进程
        pthread_cond_signal(&pool->tp_cond_full);

        // 6.释放线程池互斥锁
        pthread_mutex_unlock(&pool->tp_mutex_pool);

        // 7.执行任务
        task.function(task.arg);// 执行任务
		printf("[DEBUG] Thread %ld end working...\n",pthread_self());

    }
}

/* 4. 线程池的销毁*/
int thread_pool_destroy(tpool_t *pool) {
    if (!pool)
        return -1;

    pool->tp_shutdown = true;

    // 如果线程处理睡眠状态，无法判断标志，需要将线程唤醒
    //唤醒阻塞的工作线程,判断销毁标志为 true 时,线程会退出
    for (int i = 0;i < pool->tp_number_of_threads;i++){
        pthread_cond_signal(&pool->tp_cond_empty);
    }

    // 释放任务队列空间
    if (pool->tp_task)
        free(pool->tp_task);
    
    // 释放保存线程 id 空间
    if (pool->tp_work_thread_ids)
        free(pool->tp_work_thread_ids);
    
    // 释放互斥锁与条件变量
    pthread_mutex_destroy(&pool->tp_mutex_pool);
    pthread_cond_destroy(&pool->tp_cond_empty);
    pthread_cond_destroy(&pool->tp_cond_full);
    
    // 释放线程池的数据结构空间
    free(pool);
    pool = NULL;
    return 0;
}