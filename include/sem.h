#ifndef __SEM_H_
#define __SEM_H_

#include "common.h"

extern int sem_create(int nsems,unsigned short values[]); //创建信号量集合
extern int sem_p(int semid,int semnum);// 占用资源
extern int sem_v(int semid,int semnum);// 释放资源
extern int sem_del(int semid); // 删除信号量集合

#endif
