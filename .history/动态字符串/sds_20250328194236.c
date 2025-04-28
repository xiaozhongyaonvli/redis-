#include <stddef.h>  // 为 NULL 宏提供支持

typedef struct{    // SDS字符串
    long len;   // 字符串已存入长度
    long remaining;  // 剩余空间
    char buf[]; // 字节数组
} SDS;


// 初始化SDS(初始时数组全空)
SDS* sds_init(long size){
    // 为结构体SDS和柔性数组buf分配内存
    SDS* sds = (SDS*)malloc(sizeof(SDS)+sizeof(char)*(size+1)); // 这里+1是给/0填空
    sds->buf = '/0';
    if (!sds){ // 分配内存失败
        return;
    }
}

// 扩大数组
void sds_grow(SDS* sds, long size){

}


void sds_cpy(SDS* sds, char* ch){
    char* temp_ch = ch; // 临时字符指针
    long ch_size = 0;   // 记录字符大小
    while (*temp_ch++ != '\0') ch_size++;
    if (ch_size > (sds->len+sds->free)) {

    }
}
