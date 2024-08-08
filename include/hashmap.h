#ifndef _HASHMAP_H
#define _HASHMAP_H
#include "common.h"
#define HTABLE_LEN 7

/* 链地址法-链表的集合-二级指针，保存每串链表的指针集合 */
typedef struct datatype{
    char topic[32]; // 主题
    pid_t pid;      // 订阅进程 id 
}data_t;

typedef struct node {
    data_t data;
    struct node* next;
}hashtable_t;

extern hashtable_t** create_hashtable();
extern void insert_data_hash(hashtable_t** map,char* topic,data_t val);
extern void printf_hash_table(hashtable_t** map);
extern int search_hash_table(hashtable_t** map, char* topic, pid_t result[]);
extern void hashmap_test();
extern void free_map(hashtable_t** map);
#endif
