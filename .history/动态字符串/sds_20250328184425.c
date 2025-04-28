struct SDS {    // SDS字符串
    long len;   // 字符串长度
    long remaining;  // 剩余空间
    char buf[]; // 字节数组
};

