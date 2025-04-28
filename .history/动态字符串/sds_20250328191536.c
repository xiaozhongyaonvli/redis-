#include <stddef.h>  // 为 NULL 宏提供支持

typedef struct{    // SDS字符串
    long len;   // 字符串长度
    long remaining;  // 剩余空间
    char buf[]; // 字节数组
} SDS;


// 初始化SDS(初始时数组全空)
SDS* init_SDS(long size){
    SDS* sds = (SDS*)malloc(sizeof(SDS)+sizeof(char)*size);

}

void cpy_SDS(SDS* sds, char ch[]){
    char* temp_ch = NULL;

}
