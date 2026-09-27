#include "md5_utils.h"
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <sstream>

// MD5常量表
const unsigned int MD5Utils::S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

const unsigned int MD5Utils::T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

std::string MD5Utils::calculateMD5(const std::string& input) {
    // 初始化MD5状态
    unsigned int state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    
    // 计算消息长度（以位为单位）
    unsigned long long bitLen = input.length() * 8;
    
    // 填充消息
    size_t originalLen = input.length();
    size_t paddedLen = ((originalLen + 8) / 64 + 1) * 64;
    
    unsigned char* paddedMsg = new unsigned char[paddedLen];
    memset(paddedMsg, 0, paddedLen);
    memcpy(paddedMsg, input.c_str(), originalLen);
    
    // 添加填充位
    paddedMsg[originalLen] = 0x80;
    
    // 添加原始长度（小端序）
    for (int i = 0; i < 8; i++) {
        paddedMsg[paddedLen - 8 + i] = (bitLen >> (i * 8)) & 0xFF;
    }
    
    // 处理每个512位块
    for (size_t i = 0; i < paddedLen; i += 64) {
        transform(state, paddedMsg + i);
    }
    
    delete[] paddedMsg;
    paddedMsg = NULL;
    
    // 将状态转换为十六进制字符串
    std::ostringstream oss;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            oss << std::hex << std::setfill('0') << std::setw(2) 
                << ((state[i] >> (j * 8)) & 0xFF);
        }
    }
    
    return oss.str();
}

static unsigned int leftRotate(unsigned int x, unsigned int c) {
    return (x << c) | (x >> (32 - c));
}

void MD5Utils::transform(unsigned int state[4], const unsigned char block[64]) {
    unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
    unsigned int M[16]={};
    
    decode(M, block, 64);
    
    // 四轮运算
    for (int i = 0; i < 64; i++) {
        unsigned int f, g;
        
        if (i < 16) {
            f = (b & c) | ((~b) & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }
        
        unsigned int temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + T[i] + M[g]), S[i]);
        a = temp;
    }
    
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void MD5Utils::encode(unsigned char* output, const unsigned int* input, unsigned int len) {
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = input[i] & 0xFF;
        output[j + 1] = (input[i] >> 8) & 0xFF;
        output[j + 2] = (input[i] >> 16) & 0xFF;
        output[j + 3] = (input[i] >> 24) & 0xFF;
    }
}

void MD5Utils::decode(unsigned int* output, const unsigned char* input, unsigned int len) {
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = ((unsigned int)input[j]) | 
                    (((unsigned int)input[j + 1]) << 8) |
                    (((unsigned int)input[j + 2]) << 16) | 
                    (((unsigned int)input[j + 3]) << 24);
    }
}
