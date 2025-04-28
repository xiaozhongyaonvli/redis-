#include <stddef.h>  // 为 NULL 宏提供支持

typedef struct{    // SDS字符串
    long len;   // 字符串已存入长度
    long remaining;  // 剩余空间
    char* buf; // 字节数组
} SDS;


// 初始化SDS(初始时数组全空)
SDS* sds_init(long size){
    // 为结构体SDS和柔性数组buf分配内存(这里的sizeof(SDS)指的是len free buf字节指针这三个的大小)
    SDS* sds = (SDS*)malloc(sizeof(SDS)+sizeof(char)*(size+1)); // 这里+1是给/0填空
    if (!sds){ // 分配内存失败
        return;
    }
    sds->buf = (char*)(sds+1) // 将字节指针指向分配的字节数组
    memset(sds->buf,'\0',size+1);
    return sds;
}

// 扩大数组
void sds_grow(SDS* sds, long ch_size, const char* ch){
    // 获取新的数组占用空间大小
    int memory_size = ch_size*sizeof(char)/1024*1024;
    // (如果修改后SDS长度小于1MB,程序分配与len属性同样大小的未使用空间)
    if (memory_size < 1) {
        SDS* new_sds = sds_init(2*memory_size+1); // 注意单独的空字符空间
        memcpy(new_sds->buf,ch,ch_size);
    }

    

}


void sds_cpy(SDS* sds, char* ch){
    char* temp_ch = ch; // 临时字符指针
    long ch_size = 0;   // 记录字符大小
    while (*temp_ch++ != '\0') ch_size++;
    if (ch_size > (sds->len+sds->free)) {

    }
}
