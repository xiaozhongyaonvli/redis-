#include <stdint.h>
#include <stdlib.h>

typedef struct dictEntry {
    // 键
    void *key;
    // 值
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
    } v;
    // 指向下个哈希表节点，形成链表
    struct dictEntry *next;
} dictEntry;

typedef struct dictht {
    // 哈希表数组
    dictEntry **table;
    // 哈希表大小
    unsigned long size;
    // 哈希表大小编码，用于计算索引值(总是等于 size - 1)
    unsigned long sizemask;
    // 该哈希表已有节点的数量
    unsigned long used;
} dictht;

typedef struct dictType {
    // 计算哈希值的函数
    unsigned int (*hashFunction) (const void *key);
    // 复制键的函数
    void *(*keyDup) (void *privdata, const void *key);
    // 复制值的函数
    void *(*valDup) (void *privdata, const void *obj);
    // 对比键的函数
    int (*keyCompare) (void *privdata, const void *key1, const void *key2);
    // 销毁键的函数
    void (*keyDestructor) (void *privdata, void *key);
    // 销毁值的函数
    void (*valDestructor) (void *privdata, void *obj);
} dictType;

typedef struct dict {
    // 类型特定函数
    dictType *type;
    // 私有数据
    void *privdata;
    // 哈希表
    dictht ht[2];
    // rehash索引
    // 当rehash不在进行时，值为-1
    int rehashidx;
} dict;

/* 创建一个新的哈希表并初始化 */
dictht* dicthtCreate(){
    dictht* new_dictht = malloc(sizeof(dictht));
    new_dictht->size = 0;
    new_dictht->table = NULL;
    new_dictht->sizemask = -1;
    new_dictht->used = 0;
    return new_dictht;
}

/**
 * @brief 创建一个新的字典
 * 
 * @param type     类型特定函数
 * @param privdata 私有数据
 * @return dict*   返回指针指向新建字典
 */
dict* dictCreate (dictType *type, void *privdata){
    // 这里已经包含了ht[2]的空间，并且是连续的空间
    dict* new_dict = malloc(sizeof(dict));
    new_dict->type = type;
    new_dict->privdata = privdata;
    dictht* ht0 = dicthtCreate();
    dictht* ht1 = dicthtCreate();
    // 内容拷贝(结构体赋值)
    new_dict->ht[0] = *ht0;
    new_dict->ht[1] = *ht1;
    free(ht0);
    free(ht1);
    new_dict->rehashidx = -1;
    return new_dict;
}

/**
 * @brief 将给定的键值对添加到字典里面
 * 
 * @param dict     字典
 * @param raw_key  待插入键
 * @param raw_val  待插入值
 */
void dictAdd (dict* dict, void *raw_key, void *raw_val){
    if (dict->rehashidx != -1){
        dictEntry* move_dictEntry = dict->ht[0].table[rehashidx]
    }
}