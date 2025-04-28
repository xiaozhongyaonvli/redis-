#include <stdlib.h>

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
    void (*free) (void *ptr);
    // 节点值对比函数
    int (*match) (void *ptr,void *key);
} list;

// 将给定得函数设置为链表节点值复制函数
void listSetDupMethod(list* l, void *(*new_dup)(void*)){
    l->dup = new_dup;
}

// 返回链表当前正在使用得节点值复制函数
void* (*listGetDupMethod(list* l))(void*){
    return l->dup;
}

// 将给定得函数设置为链表的节点值释放函数
void listSetFreeMethod(list* l, void *(*new_free)(void*)){
    l->free=new_free;
}

// 返回链表当前正在使用的节点释放函数
void (*listGetFreeMethod(list* l))(void*){
    return l->free;
}

// 将给定的函数设置为链表的节点值对比函数
void listSetMatchMethod(list* l, int(*new_match)(void*,void*)){
    l->match = new_match;
}

// 返回链表当前正在使用的节点值对比函数
int (*listGetMatchMethod(list* l))(void*,void*){
    return l->match;
}

// 返回链表的长度(包含了多少个节点)
int listLength(list* l){
    return l->len;
}

// 返回链表的表头节点
listNode* listFirst(list* l){
    return l->head;
}

// 返回链表的表尾节点
listNode* listLast(list* l){
    return l->tail;
}

// 返回给定节点的前置节点
listNode* listPrevNode(listNode* node){
    return node->prev;
}

// 返回给定节点的后置节点
listNode* listNextNode(listNode* node){
    return node->next;
}

// 返回给定节点目前正在保存的值
void* listNodeValue(listNode* node){
    return node->value;
}

// 创建一个不包含任何节点的新链表
list* listCreate(){
    list* l = (list*)malloc(sizeof(list));
    if(l == NULL){
        return NULL; // 内存分配失败
    }
    l->head = NULL;
    l->tail = NULL;
    l->len = 0;
    l->dup = NULL;
    l->free = NULL;
    l->match = NULL;
    return l;
}

// 将一个包含给定值的新节点添加到给定链表的表头
void listAddNodeHead(list* l, listNode* node){
    listNode* oldHead = l->head;
    oldHead->prev = node;
    node->next = oldHead;
    node->prev = NULL;
    l->head = node;
    l->len = l->len+1;
}