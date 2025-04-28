#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{     // SDS字符串
    long len;       // 字符串已存入长度
    long remaining;  // 剩余空间
    char* buf;      // 字节数组
} SDS;


/**
 * @brief 初始化SDS(初始时数组全空)
 * 
 * @param size  新建数组大小
 * @return SDS* 返回新创建的SDS
 */
SDS* sds_init(long size){

    // 为结构体SDS和柔性数组buf分配内存(这里的sizeof(SDS)指的是len free buf字节指针这三个的大小)
    SDS* sds = (SDS*)malloc(sizeof(SDS)+sizeof(char)*(size+1)); // 这里+1是给/0填空
    if (!sds){ // 分配内存失败,返回NULL
        return NULL;
    }
    //初始化

    sds->buf[0] = '\0'; // 将字节指针指向分配的字节数组
    memset(sds->buf,'\0',size+1); // 将字节数组初始化全部置为'\0'
    sds->len = 0;
    sds->remaining = size;

    // 返回创建的结构体
    return sds;
}


/**
 * @brief 扩大数组
 * 
 * @param sds      旧的SDS
 * @param ch_size  修改后的字节数组大小
 * @param ch       修改后的数组(如果是添加操作则为旧数组+插入数组，如果为覆盖操作则为覆盖数组)
 * @return SDS*    返回新创建的SDS
 */
SDS* sds_grow(SDS* sds, long ch_size, const char* ch){

    // 获取新的数组占用空间大小
    int memory_size = ch_size*sizeof(char)/1024*1024;

    // 如果修改后SDS长度小于1MB,程序分配与len属性同样大小的未使用空间 即 修改后 * 2 + 1byte
    SDS* new_sds;
    if (memory_size < 1)         new_sds = sds_init(2*ch_size+1); // 注意单独的空字符空间 else  //如果修改后SDS长度大于等于1MB，那么程序将分配 修改后+1MB+1byte
    else if (memory_size >= 1)   new_sds = sds_init(ch_size+1024*1024*1+1);

    // 将修改后的字节数组赋值给新的sds
    memcpy(new_sds->buf,ch,ch_size);
    free(sds);       // 删除旧的sds，防止内存泄漏

    return new_sds;  // 返回新的sds;
}

/**
 * @brief 保留SDS给定区间内的数据,不在区间内的数据会被覆盖或者清除
 * 
 * @param sds        需要处理的SDS
 * @param buf_size  保留的区间长度
 * @return SDS*     返回新创建的SDS
 */
SDS* sds_range(SDS* sds, long buf_size){

    // 检查参数合法性，如果小于0或者大于此时存的字符串直接报错，并返回空对象
    if (buf_size > 0 || buf_size>sds->len){
        fprintf(stderr,"Error: buf_zie (%ld) is out of range (0 - %ld)\n",buf_size,sds->len);
        return NULL;
    }

    // 重新设置已存入字符串长度
    sds->len = buf_size;

    // 清空字节数组其余部分
    memset(sds->buf+buf_size,'\0',sds->len-buf_size);

    // 返回清除过后SDS
    return sds;

}

/**
 * @brief 将给定的C字符复制到SDS里面，覆盖SDS原有的字符串
 * 
 * @param sds   需要处理的SDS
 * @param ch    C字符串
 * @return SDS* 返回处理后的SDS
 */
SDS* sds_cpy(SDS* sds, const char* ch){

    long ch_size = 0;   // 记录字符大小
    while (ch[ch_size] != '\0') ch_size++; // 循环遍历，得到C字符串长度

    // 如果C字符串长度大于sds的,先扩充sds，防止缓冲区溢出
    if (ch_size > (sds->len+sds->remaining))  sds = sds_grow(sds,ch_size,ch);

    sds_range(sds,0);                // 清空SDS字节数组
    memcpy(sds->buf,ch,ch_size);     // 将C字符串存入SDS

    // 更新SDS的数据
    sds->remaining = sds->len+sds->remaining-ch_size;
    sds->len = ch_size;

    return sds;       // 返回处理后的SDS
}

/**
 * @brief 比较两个SDS字符串是否相同
 * 
 * @param sds1    sds字符串1
 * @param sds2    sds字符串2
 * @return true   表示相同
 * @return false  表示不同
 */
bool sds_cmp(SDS* sds1, SDS* sds2){
    // 如果两者长度不同，则一定不同，直接返回false
    if (sds1->len != sds2->len) return false;
    // 逐位比较，如果有不同，直接返回false
    for (int i=0; i<sds1->len; ++i)
        if (sds1->buf[i] != sds2->buf)  return false;
    // 两个字符串相同，返回true
    return true;
}