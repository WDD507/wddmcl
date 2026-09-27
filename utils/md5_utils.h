#ifndef MD5_UTILS_H
#define MD5_UTILS_H

#include <string>

class MD5Utils {
public:
    MD5Utils() = delete;
    MD5Utils(const MD5Utils &) = delete;
    MD5Utils(MD5Utils &&) = delete;
    MD5Utils &operator=(const MD5Utils &) = delete;
    MD5Utils &operator=(MD5Utils &&) = delete;

    // 计算字符串的MD5值，返回32位小写16进制字符串
    static std::string calculateMD5(const std::string& input);
    
private:
    // MD5内部实现所需的常量和结构
    static const unsigned int S[64];
    static const unsigned int T[64];
    
    // MD5核心算法函数
    static void transform(unsigned int state[4], const unsigned char block[64]);
    static void encode(unsigned char* output, const unsigned int* input, unsigned int len);
    static void decode(unsigned int* output, const unsigned char* input, unsigned int len);
};

#endif // MD5_UTILS_H
