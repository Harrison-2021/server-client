#include "shm.h"

/* 1.创建共享内存
传入要创建共享内存的大小，并将共享内存id以参数传回 */
enum shm_creat_status shm_create(size_t size,int *p_shmid) {
    key_t key;
    int shmid;

    key = ftok(PATHNAME,PRO_ID);

    /* [参数shmflg]：如果要创建新的共享内存，需要使用IPC_CREAT，IPC_EXCL，
    如果是已经存在的，可以使用IPC_CREAT或直接传0。
    IPC_CREAT  如果共享内存不存在，则创建一个共享内存，否则打开操作。
    IPC_EXCL  只有在共享内存不存在的时候，新的共享内存才建立，否则就产生错误。
    */
    // 先判断是否创建过，创建过，无需重复创建
    shmid = shmget(key,size,0); 
    if(shmid == -1) {
        shmid = shmget(key, size, IPC_CREAT | 0644);
        if(shmid == -1) {
            perror("[ERROR]: shmget() ");
            return SHM_CREAT_ERROR;
        }
        *p_shmid = shmid;
        return SHM_CREAT_NEW;
    } else {
        *p_shmid = shmid;
        return SHM_HAS_EXIST;
    }
   
}

/* 提供共享内存映射接口-将当前进程地址映射到共享内存 
    返回映射的地址*/
void *shm_at(int shmid) {
    void* addr = NULL;

    addr = shmat(shmid, NULL, 0);
    if(addr == (void*)-1) {
        perror("[ERROR]: shmat() ");
        return NULL;
    }

    return addr;
}
int shm_dt(const void *shmaddr) {
    return shmdt(shmaddr);
}

int shm_del(int shmid) {
    return shmctl(shmid, IPC_RMID, NULL);
}
