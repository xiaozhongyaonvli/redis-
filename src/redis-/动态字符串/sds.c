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
 * @note 时间复杂度：O(n) —— 主要由 memset 初始化缓冲区消耗，n 为 size
 */
SDS* sds_init(long size) {
    // 为结构体SDS和柔性数组buf分配内存(这里的sizeof(SDS)指的是len、remaining和buf指针这几个的大小)
    SDS* sds = (SDS*)malloc(sizeof(SDS) + sizeof(char) * (size + 1)); // 这里+1是给'\0'留空间
    if (!sds) { // 分配内存失败,返回NULL
        return NULL;
    }
    // 初始化
    sds->buf[0] = '\0';            // 将字节指针指向分配的字节数组
    memset(sds->buf, '\0', size + 1);  // 将字节数组初始化全部置为'\0'
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
 * @note 时间复杂度：O(n) —— 主要由 memcpy 和 sds_init 的内存初始化消耗，n 为 ch_size
 */
SDS* sds_grow(SDS* sds, long ch_size, const char* ch) {
    // 获取新的数组占用空间大小（单位：MB）
    int memory_size = ch_size * sizeof(char) / 1024 / 1024;

    SDS* new_sds;
    // 如果修改后SDS长度小于1MB,则分配大小为 修改后*2 + 1 byte 的空间
    if (memory_size < 1)
        new_sds = sds_init(2 * ch_size + 1);
    // 如果修改后SDS长度大于等于1MB,分配大小为 修改后+1MB+1 byte 的空间
    else if (memory_size >= 1)
        new_sds = sds_init(ch_size + 1024 * 1024 * 1 + 1);

    // 将修改后的字节数组赋值给新的SDS
    memcpy(new_sds->buf, ch, ch_size);
    sds_free(sds);    // 删除旧的SDS，防止内存泄漏

    return new_sds;  // 返回新的SDS
}

/**
 * @brief 保留SDS给定区间内的数据, 不在区间内的数据会被覆盖或者清除
 * 
 * @param sds      需要处理的SDS
 * @param buf_size 保留的区间长度
 * @return SDS*    返回处理后的SDS；参数不合法时返回NULL
 * @note 时间复杂度：O(n) —— 主要由 memset 清空多余部分消耗，n 为 (sds->len - buf_size)
 */
SDS* sds_range(SDS* sds, long buf_size) {
    // 检查参数合法性，如果小于0或者大于当前存储的字符串长度直接报错，并返回空对象
    if (buf_size < 0 || buf_size > sds->len) {
        fprintf(stderr, "Error: buf_size (%ld) is out of range (0 - %ld)\n", buf_size, sds->len);
        return NULL;
    }

    // 重新设置已存入字符串长度
    // 注意：调用 memset 前要保存原始的 sds->len，否则 memset 的长度计算可能不正确
    long old_len = sds->len;
    sds->len = buf_size;

    // 清空字节数组其余部分（如果有）
    if (old_len > buf_size) {
        memset(sds->buf + buf_size, '\0', old_len - buf_size);
    }

    // 返回处理后的SDS
    return sds;
}

/**
 * @brief 将给定的C字符串复制到SDS里面，覆盖SDS原有的字符串
 * 
 * @param sds   需要处理的SDS
 * @param ch    C字符串
 * @return SDS* 返回处理后的SDS
 * @note 时间复杂度：O(n) —— 主要由遍历计算长度、可能的扩容和 memcpy 消耗，n 为字符串长度
 */
SDS* sds_cpy(SDS* sds, const char* ch) {
    long ch_size = 0;   // 记录C字符串长度
    while (ch[ch_size] != '\0') {
        ch_size++;
    }

    // 如果C字符串长度大于SDS当前容量，先扩充SDS，防止缓冲区溢出
    if (ch_size > (sds->len + sds->remaining)) {
        sds = sds_grow(sds, ch_size, ch);
    }

    sds_range(sds, 0);             // 清空SDS字节数组
    memcpy(sds->buf, ch, ch_size); // 将C字符串存入SDS

    // 更新SDS的数据：剩余空间和当前长度
    sds->remaining = sds->len + sds->remaining - ch_size;
    sds->len = ch_size;

    return sds;       // 返回处理后的SDS
}

/**
 * @brief 比较两个SDS字符串是否相同
 * 
 * @param sds1 SDS字符串1
 * @param sds2 SDS字符串2
 * @return true  表示相同
 * @return false 表示不同
 * @note 时间复杂度：O(n) —— 逐字符比较，n 为字符串长度
 */
bool sds_cmp(SDS* sds1, SDS* sds2) {
    // 如果两者长度不同，则一定不同，直接返回false
    if (sds1->len != sds2->len) return false;
    // 逐位比较，如果有不同，直接返回false
    for (int i = 0; i < sds1->len; ++i) {
        if (sds1->buf[i] != sds2->buf[i]) return false;
    }
    // 两个字符串相同，返回true
    return true;
}

/**
 * @brief 释放SDS
 * 
 * @param sds 需要释放的SDS
 * @note 时间复杂度：O(1) —— 内存释放通常认为是O(1)（具体依赖于内存管理实现）
 */
void sds_free(SDS* sds) {
    free(sds->buf);  // 释放SDS中分配的缓冲区
    free(sds);       // 释放SDS结构体
}

/**
 * @brief 返回SDS的已使用空间字节数
 * 
 * @param sds 需要查询的SDS
 * @return long 已使用空间字节数
 * @note 时间复杂度：O(1)
 */
long sds_len(SDS* sds) {
    return sds->len;
}

/**
 * @brief 返回SDS的未使用空间字节数
 * 
 * @param sds 需要查询的SDS
 * @return long 未使用空间字节数
 * @note 时间复杂度：O(1)
 */
long sds_vail(SDS* sds) {
    return sds->remaining;
}