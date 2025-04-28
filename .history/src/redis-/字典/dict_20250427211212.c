#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define dictAddAuto(dict, key, val) \
    _Generic((val), \
        uint64_t: dictAdd((dict), (key), (void*)(uintptr_t)(val), 1), \
        int64_t: dictAdd((dict), (key), (void*)(intptr_t)(val), 2), \
        default: dictAdd((dict), (key), (void*)(val), 0) \
    )

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
    // 哈希表数组指针 (指向哈希表数组，本身只有八字节)
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
 * @brief 将待插入键值对插入哈希表(ht)中
 * 
 * @param dict    处理字典 
 * @param flag    0表示ht[0],1表示ht[1]
 * @param raw_key 待插入键
 * @param raw_val 待插入值
 */
void dictDictEntryInsert (dict* dict, bool flag, void *raw_key, void *raw_val){
    // 创建一个键值对
    dictEntry* new_dictEntry = mallo(sizeof(dictEntry));
    new_dictEntry->key = dict->type->keyDup(dict->privdata,raw_key);
    // TODO 这里需要解决value的泛型问题
    new_dictEntry->v.val  = (void*)dict->type->valDup(dict->privdata,raw_val);
    // 计算出待插入键值对的pos
    unsigned int pos = dict->type->hashFunction(raw_key) & dict->ht[flag].sizemask;
    // 将待插入的放置链表头部
    *(dict->ht[flag].table) = &new_dictEntry;
    // 如果原链表不为空，将其链接到新键值对后
    // 取出pos位置的键值对或者键值对链表(注意可能取出来为空)
    dictEntry* head_dictEntry = dict->ht[flag].table[pos];
    if (head_dictEntry != NULL) new_dictEntry->next = head_dictEntry;
}

/**
 * @brief         判断是否在进行rehash，如果在rehash进行复制操作
 * 
 * @param dict    待处理字典
 */
void dictRehash(dict* dict){
        // 判断是否在rehash，等于-1表示再rehash
        if (dict->rehashidx != -1){
            // 取出该次需移动的键值对(注意可能为链表)
            dictEntry* move_dictEntry = dict->ht[0].table[dict->rehashidx];
            // 遍历该次移动的键值对
            while (move_dictEntry != NULL) {
                dictEntry* temp_dictEntry = move_dictEntry->next;
                unsigned int hash_pos = dict->type->hashFunction(move_dictEntry->key) & dict->ht->sizemask;
                dictEntry* head_dictEntry = dict->ht[1].table[hash_pos];
                move_dictEntry->next = head_dictEntry;
                dict->ht[1].table[hash_pos] = move_dictEntry;
                move_dictEntry = temp_dictEntry;
            }
            // rehashidx 索引 + 1
            dict->rehashidx = dict->rehashidx + 1;
            // 更新两个哈希表键值对数量
            dict->ht[0].size--;
            dict->ht[1].size++;
            // 判断，如果ht[0]的size变为0，rehash结束
            if (dict->ht[0].size == 0) {
                // 修改rehashidx为-1，标志rehash结束
                dict->rehashidx = -1;
                // 交换ht[0],ht[1]
                dictht* temp_dictht = &(dict->ht[0]);
                dict->ht[0] = dict->ht[1];
                dict->ht[1] = *temp_dictht;
                // 释放临时指针
                free(temp_dictht);
            }
        }
}

/**
 * @brief 将给定的键值对添加到字典里面
 * 
 * @param dict     字典
 * @param raw_key  待插入键
 * @param raw_val  待插入值
 */
void dictAdd (dict* dict, void *raw_key, void *raw_val){
    // 判断是否在rehash,并进行相关处理
    dictRehash(dict);
    // 进行插入操作,(这里如果是正在进行rehash操作则选择插入ht[1]，否则ht[0])
    dictDictEntryInsert(dict, dict->rehashidx == -1 ? 0 : 1, raw_key, raw_val);
}

/**
 * @brief 将给定键值对添加到字典里面，如果键已经存在于字典，那么用键值对取代原有的值
 * 
 * @param dict      待插入字典
 * @param raw_key   待插入键
 * @param raw_value 待插入值
 */
void dictReplace(dict* dict, void* raw_key, void* raw_val){
    // 创建一个键值对
    dictEntry* new_dictEntry = mallo(sizeof(dictEntry));
    new_dictEntry->key = dict->type->keyDup(dict->privdata,raw_key);
    // TODO 这里需要解决value的泛型问题
    new_dictEntry->v.val  = (void*)dict->type->valDup(dict->privdata,raw_val);
    // 判断是否在rehash,并进行相关处理
    dictRehash(dict);
    bool flag = dict->rehashidx == -1 ? 0 : 1;
    // 计算出待插入键值对的pos
    unsigned int pos = dict->type->hashFunction(raw_key) & dict->ht[flag].sizemask;
    // 取出pos位置的键值对或者键值对链表(注意可能取出来为空)
    dictEntry* head_dictEntry = dict->ht[flag].table[pos];
    // 用于记录是否该键已经存在于字典，如果是则值变为1
    bool changeFlag = 0;
    // 若链表不为空，遍历键值对链表，查找是否存在该键值对，存在则修改值和changeFlag
    while (head_dictEntry != NULL){
        
    }
}