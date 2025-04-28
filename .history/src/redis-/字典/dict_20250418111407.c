#include <stdint.h>

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
    unsigned int (*hashFunction) {const void *key}
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

