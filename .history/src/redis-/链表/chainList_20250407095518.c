typedef struct listNode {
    // 前置节点
    struct listNode* prev;
    // 后置节点
    struct listNode* next;
    // 节点的值
    void* value;
}listNode;  

typedef struct list {
    // 表头节点
    listNode* head;
    // 表尾节点
    listNode* tail;
    // 链表所包含的节点数量
    unsigned long len;

    // 节点值复制函数
    void *(*dup)  (void *ptr);
    // 节点值释放函数
    void *(*free) (void *ptr);
    // 节点值对比函数
    int (*match) (void *ptr,void *key);
} list;

// 将给定得函数设置为链表节点值复制函数
void listSetDupMethod(list* list, void *(*new_dup)(void*)){
    list->dup = new_dup;
}

// 返回链表当前正在使用得节点值复制函数
void* (*listGetDupMethod(list* list))(void*){
    return list->dup;
}

// 将给定得函数设置为链表的节点值释放函数
void listSetFreeMethod(list* list, void *(*new_free)(void*)){
    list->free=new_free;
}

// 返回链表当前正在使用的节点释放函数
void* (*listGetFreeMethod(list* list))(void*){
    
}