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
    // 如果链表或新节点为空直接返回
    if (l == NULL || node == NULL) return;
    // 指定新节点 prev和next状态
    node->prev = NULL;
    node->next = l->head;
    // 如果链表为空，那么新节点同时也是尾节点
    if (l->head != NULL){
        l->head->prev = node;
    } else {
        l->tail = node;
    }
    // 将新节点插入链表，并且计数+1
    l->head = node;
    l->len = l->len+1;
}

// 将一个包含给定值的新节点添加到给定链表的表尾
void listAddNodeTail(list* l, listNode* node) {
    // 如果链表或新节点为空则直接返回
    if (l == NULL || node == NULL) return;
    
    // 设置新节点的指针状态：next 指向 NULL，因为它将成为新尾节点
    node->next = NULL;
    // prev 指向当前链表的尾节点
    node->prev = l->tail;
    
    // 如果链表不为空，则更新原尾节点的 next 指针
    if (l->tail != NULL) {
        l->tail->next = node;
    } else {
        // 如果链表为空，新节点同时也是头节点
        l->head = node;
    }
    
    // 更新链表的尾节点为新节点
    l->tail = node;
    
    // 增加链表的节点计数
    l->len = l->len + 1;
}

// 将一个包含给定值的新节点添加到给定节点的之前或者之后
void listInsertNode(list* l, listNode* target, listNode* new_node, int before) {
    if (l == NULL || target == NULL || new_node == NULL) {
        // 参数检查
        return;
    }

    if (before) {
        // 插入到目标节点之前
        new_node->prev = target->prev;
        new_node->next = target;
        if (target->prev != NULL) {
            target->prev->next = new_node;
        } else {
            // 如果目标节点是头节点
            l->head = new_node;
        }
        target->prev = new_node;
    } else {
        // 插入到目标节点之后
        new_node->prev = target;
        new_node->next = target->next;
        if (target->next != NULL) {
            target->next->prev = new_node;
        } else {
            // 如果目标节点是尾节点
            l->tail = new_node;
        }
        target->next = new_node;
    }
    l->len++;
}

// 查找并返回链表中包含给定值的节点
listNode* listFind(list* l, void* value) {
    if (l == NULL || value == NULL) return NULL;
    listNode* current = l->head;
    while (current != NULL) {
        if (l->match(current->value, value)) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 从链表中删除给定节点
// 删除链表中的节点 node
void listDeleteNode(list* l, listNode* node) {
    if (l == NULL || node == NULL) return;
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        l->head = node->next; // 删除的是头节点
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        l->tail = node->prev; // 删除的是尾节点
    }
    free(node);
    l->len--;
}

// 释放给定链表，以及链表中的所有节点
void listRelease(list* l) {
    if (l == NULL) return;
    listNode* current = l->head;
    while (current != NULL) {
        listNode* next = current->next;
        if (l->free != NULL) {
            l->free(current->value);
        }
        free(current);
        current = next;
    }
    free(l);
}