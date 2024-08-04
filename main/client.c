#include <stdio.h>
#include <string.h>

#include "shm.h"

int main(void)
{
    int shmid;
    enum shm_creat_status shm_status;
    void *addr = NULL;
    char buffer[16] = {0};

    shm_status = shm_create(SHM_SZ,&shmid) ;

    if (shm_status == SHM_CREAT_NEW)
        printf(" shared memory creat new.\n");
    else if (shm_status == SHM_HAS_EXIST)
        printf(" shared memory has exist.\n");
    
    addr = shm_at(shmid);
    if (addr == NULL){
        printf("shm at failed.\n");
        return -1;
    }
    
    memset(addr,'A',10);

   

    shm_dt(addr);
    return 0;
}