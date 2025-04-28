typedef struct listNode {
    // 前置节点
    struct listNode* prev;
    // 后置节点
    struct listNode* next;
    // 节点的值
    void* value;
}listNode;  

struct list {
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


    // 将给定得函数设置为链表节点值复制函数
    listSetDupMethod(void )
};