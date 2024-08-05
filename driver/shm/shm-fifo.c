#include "shm-fifo.h"

static int g_shmid;
/* 1.创建环形队列，传入数据块大小与数量 */
shm_fifo_t* shm_fifo_init(int block_sz, int block_len) {
    enum shm_creat_status status;
    int shmsz;
    unsigned short values[] = {0,block_sz,1};//空信号量，满信号量，互斥信号量
    // 1.分配环形队列数据结构空间
    shm_fifo_t* fifo = (p_shm_fifo)malloc(sizeof(shm_fifo_t));
    if(!fifo) {
        perror("[ERROR]: malloc() ");
        return NULL;
    }

    // 2.初始化环形队列
    shmsz = block_sz * block_len + sizeof(shm_head_t);// 队列大小
    status = shm_create(shmsz, &g_shmid);
    if(status != SHM_CREAT_ERROR) {
        // 初始化头节点
        fifo->head = (p_shm_head)shm_at(g_shmid);
        if(!fifo->head) return NULL;
        if(status == SHM_CREAT_NEW) {
            fifo->head->r_pos = 0;
            fifo->head->w_pos = 0;
            fifo->head->block_len = block_len;
            fifo->head->block_sz = block_sz;
            fifo->head->semid = sem_create(3, values);
            if(fifo->head->semid == -1) return NULL;
        }
        // 初始化数据区域
        fifo->payload = (char *)(fifo->head + 1);
        return fifo;
    }

    return NULL;
}

/* 2.销毁队列，释放资源 */
void shm_fifo_destroy(p_shm_fifo fifo) {
    sem_del(fifo->head->semid);
    shm_dt(fifo->head);
    shm_del(g_shmid); // 删除共享内存,shmid要定义成全局变量
    free(fifo);
}

/* 3. 写数据*/
void shm_fifo_put(p_shm_fifo fifo, const void* buf) {
    int pos = 0;

    sem_p(fifo->head->semid, SEM_FULL_ID);  // 获取满信号
    sem_p(fifo->head->semid, SEM_MUTEX_ID); // 获取互斥锁

    pos = fifo->head->w_pos * fifo->head->block_len;
    memcpy(fifo->payload + pos, buf, fifo->head->block_len);
    fifo->head->w_pos = (fifo->head->w_pos + 1) % (fifo->head->block_sz);

    sem_v(fifo->head->semid, SEM_MUTEX_ID); // 释放互斥锁
    sem_v(fifo->head->semid, SEM_EMPTY_ID); // 释放空信号

}

/* 4. 读数据 */
void shm_fifo_get(p_shm_fifo fifo, void* buf) {
    int pos = 0;

    sem_p(fifo->head->semid, SEM_EMPTY_ID);
    sem_p(fifo->head->semid, SEM_MUTEX_ID);

    pos = fifo->head->r_pos * fifo->head->block_len;
    memcpy(buf, fifo->payload + pos, fifo->head->block_len);
    fifo->head->r_pos = (fifo->head->r_pos + 1) % (fifo->head->block_sz);

    sem_v(fifo->head->semid, SEM_MUTEX_ID);
    sem_v(fifo->head->semid, SEM_FULL_ID);

}

