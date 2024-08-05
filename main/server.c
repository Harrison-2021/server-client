#include "shm-fifo.h"

typedef struct person{
    int age;
    char name[32];

}person_t;

// 服务器负责从环形队列中读取数据
int main(void)
{
    person_t person;

    shm_fifo_t *fifo = shm_fifo_init(3,sizeof(person_t));

    for(;;){
        shm_fifo_get(fifo,&person);
        printf("name = %s,age = %d\n",person.name,person.age);
        sleep(1);
    }
    return 0;
}