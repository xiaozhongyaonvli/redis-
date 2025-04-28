typedef struct{    // SDS字符串
    long len;   // 字符串长度
    long remaining;  // 剩余空间
    char* buf; // 字节数组

    void (*sdscpy)(SDS *);
} SDS;


// 初始化SDS(初始时数组全空)
void init_SDS(SDS* sds, long len, long free){
    sds->free = free;
    sds->len = 0;
    sds->buf = (char*)malloc(free * sizeof(char));

}

void cpy_SDS(SDS* sds, char* ch){
    char* temp_ch = null
}
