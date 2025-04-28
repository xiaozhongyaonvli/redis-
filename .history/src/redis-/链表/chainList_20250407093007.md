# chainList #

## 1.chainList定义 ##

listNode的定义
```c
typedef struct listNode {
    // 前置节点
    struct listNode* prev;
    // 后置节点
    struct listNode* next;
    // 节点的值
    void* value;
}listNode;  
```
多个listNode可以通过prev和next指针组成双端链表

使用list来持有链表
```c
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
    int (*match) (void *ptr,void *key)
}
```
dup,free和match(函数指针)用于实现多态链表所需的类型特定函数