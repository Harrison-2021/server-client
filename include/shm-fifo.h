#ifndef __SHM_FIFO_H_
#define __SHM_FIFO_H_

#include "shm.h"
#include "sem.h"

#define SEM_EMPTY_ID 0
#define SEM_FULL_ID 1
#define SEM_MUTEX_ID 2

typedef struct shm_head{
	int r_pos;      // 出队列的位置
	int w_pos;      // 入队列的位置

	int block_sz;   // 数据块的数量
	int block_len;  // 数据块的大小

	int semid;     // 信号量集合的id

}shm_head_t, *p_shm_head;

typedef struct shm_fifo {
    shm_head_t* head; // 环形队列头部信息
    char* payload;    // 环形队列数据
}shm_fifo_t, *p_shm_fifo;

extern shm_fifo_t* shm_fifo_init(int block_sz, int block_len);
extern void shm_fifo_destroy(p_shm_fifo fifo);
extern void shm_fifo_put(p_shm_fifo fifo, const void* buf);
extern void shm_fifo_get(p_shm_fifo fifo, void* buf);

#endif