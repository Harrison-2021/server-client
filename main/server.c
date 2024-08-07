#include "shm-fifo.h"

#if 0
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
#endif

#include "thread-pool.h"

// 任务回调函数
void taskfunc(void *arg)
{
    int num = *(int *)arg;

    printf("task thread %ld is working,number = %d\n",pthread_self(),num);

    sleep(2);
}

int main(void)
{
    tpool_t *pool = thread_pool_init(20,100);// 创建线程池 包含20个线程,执行100个任务
    int *p = NULL;
    for (int i = 0;i < 100;i++){
       
        p = (int *)malloc(sizeof(int)); // 每个队列访问独立变量，无需互斥操作
        *p = i + 100;
        thread_pool_add_task(pool,taskfunc,p);// 将任务添加到任务队列中
    }

    #if 1
    while(1){
	// 防止父进程结束，子线程自动结束
    }
    #endif

    sleep(30);

    thread_pool_destroy(pool);

    return 0;
}