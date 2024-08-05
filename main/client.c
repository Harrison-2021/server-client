#include "shm-fifo.h"

typedef struct person{
    int age;
    char name[32];

}person_t;

// 创建两个子进程向服务器发送数据

int main(void)
{
    int i;
    pid_t cpid;
    // 创建环形队列
    shm_fifo_t *fifo = shm_fifo_init(3,sizeof(person_t));
    person_t  person;

    cpid = fork();
    if (cpid == -1){
        perror("[ERROR]: fork()");
        exit(EXIT_FAILURE);
    }else if (cpid == 0){ // 子进程A
        for (i = 0;i < 10;i++){
            strcpy(person.name,"lisi");
            person.age = 20;
            shm_fifo_put(fifo,&person); // 数据写入环形队列
            // sleep(1);
        }

        exit(EXIT_SUCCESS);
    }else if (cpid > 0){
        cpid = fork();
        if (cpid == -1){
            perror("[ERROR]: fork()");
            exit(EXIT_FAILURE);
            
        }else if (cpid == 0){ // 子进程B
            for (i = 0;i < 10;i++){
                strcpy(person.name,"zhangsan");
                person.age = 30;
                shm_fifo_put(fifo,&person);
                // sleep(2);
            }
            exit(EXIT_SUCCESS);
        }else if(cpid > 0){  // 释放子进程资源
            wait(NULL);
            wait(NULL); // 阻塞等待子进程结束
        }
    }

    return 0;
}
