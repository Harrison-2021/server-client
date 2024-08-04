#ifndef __SHM_H_
#define __SHM_H_

#include "common.h"

#define PATHNAME "."
#define PRO_ID 101   // 不超过8位

#define SHM_SZ 256

/*
共享内存创建状态:
    SHM_HAS_EXIST : 共享内存已经存在
    SHM_CREAT_NEW : 共享内存为新创建
    SHM_CREAT_ERROR ： 共享内存创建错误
用途: 用于后续环形队列区分是否为第一次创建，在第一次创建是会进行初始化
*/
enum shm_creat_status{
    SHM_HAS_EXIST = 0,
    SHM_CREAT_NEW,    // 后续枚举成员默认都是依上个成员的枚举值依次递增的
    SHM_CREAT_ERROR,
};

extern enum shm_creat_status shm_create(size_t size,int *pshmid);// 创建共享内存

extern void *shm_at(int shmid);// 映射
extern int shm_dt(const void *shmaddr);// 解除映射
extern int shm_del(int shmid); // 删除共享内存

#endif