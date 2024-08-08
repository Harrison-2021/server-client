#include"hashmap.h"

/* 1. 创建空的哈系表-key：下标， val: 链表串头节点的地址 */
hashtable_t** create_hashtable() {
    int size = HTABLE_LEN * sizeof(hashtable_t*);
    hashtable_t** map = (hashtable_t**)malloc(size);
    if(!map) {
        perror("[ERROR]: malloc map ");
        return NULL;
    }
    memset(map,0,size);
    // 每个链表指针地址初始化为NULL
    for(int i = 0; i < HTABLE_LEN; i++) map[i] = NULL;

    return map;
}

int hashcode(const char *string) {
    int hashcode = 0;

    while (*string){
        hashcode += *string++;  
    }

    return hashcode;
}
/* 2. 添加元素-key：下标,主题字符串，val: 主题绑定的pid结构体 */
void insert_data_hash(hashtable_t** map,char* topic,data_t val) {
    hashtable_t** p = NULL;   // 要插入的节点的位置：pre->next
    hashtable_t* cur = NULL; // 要插入的节点
    // 在使用cur前，一定要先在堆区分配内存
    cur = (hashtable_t*)malloc(sizeof(hashtable_t));
    if(!cur) {
        perror("[ERROR]: hashtable_t malloc ");
        return;
    }
    cur->data = val; // 不要忘记赋值了

    int key = hashcode(topic) % HTABLE_LEN;
    for(p = &(map[key]); *p; p = &(*p)->next) { // 第一次插入，p找到key下标，值NULL，放cur
        if((*p)->data.pid > val.pid) break;
    }

    cur->next = *p;  // p表示要替换的位置;cur->next = pre->next;
    *p = cur;        // pre->next = cur
    // printf("key : %d, topic : %s\n",key,map[key]->data.topic);
}
/* 3. 输出函数*/
void printf_hash_table(hashtable_t** map) {
    hashtable_t* p;
    for(int i = 0; i < HTABLE_LEN; i++) {
        printf("key = %d : ",i);
        for(p = map[i]; p; p = p->next) {
            printf("<hd%d> %s, ",hashcode(p->data.topic),p->data.topic);
        }
        printf("\n");
    }
}
/* 4. 查询
    @param result[]: 查询到对应topic的pid结果集合*/
int search_hash_table(hashtable_t** map, char* topic, pid_t result[]) {
    int cnt = 0;
    int key = hashcode(topic) % HTABLE_LEN;
    hashtable_t* p = NULL;
    for(int i = 0; i < HTABLE_LEN; i++) {
        for(p = map[i]; p; p = p->next) {
            if(strcmp(topic,p->data.topic) == 0) result[cnt++] = p->data.pid;
        }
    }
    return cnt;
}

void free_map(hashtable_t** map) {
    for(int i = 0; i < HTABLE_LEN; i++) {
        free(map[i]);
        map[i] = NULL;
    }
    free(map);
    map = NULL;
}

void hashmap_test() {
    hashtable_t **h = NULL;
    
    data_t value[] = {
        {"hello",123},
        {"hello",889},
        {"world",456},
        {"adfdfd",789},
        {"dadfdf",890},
    };

    h = create_hashtable();

    insert_data_hash(h,"hello",value[0]);
    insert_data_hash(h,"hello",value[1]);
    insert_data_hash(h,"world",value[2]);
    insert_data_hash(h,"adfdfd",value[3]);
    insert_data_hash(h,"dadfdf",value[4]);
    
    int result[5] = {0};
    int ret = search_hash_table(h,"adfdfd",result);
    if (ret != 0){
        for (int i = 0;i < ret;i++){
            printf("查询到的pid: %d\n",result[i]);
        }
    }

    printf_hash_table(h);
    free_map(h);
}