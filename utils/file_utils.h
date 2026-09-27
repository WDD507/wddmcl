#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

namespace FileUtils {
    // 创建目录
    bool createDirectory(const std::string& path);
    
    // 复制文件
    bool copyFile(const std::string& source, const std::string& dest);
    
    // 删除文件或目录
    bool removePath(const std::string& path);
    
    // 检查文件是否存在
    bool fileExists(const std::string& path);
    
    // 获取文件大小
    long long getFileSize(const std::string& path);
    
    // 解压ZIP文件
    bool extractZip(const std::string& zipFile, const std::string& destDir);
    
    // 计算文件哈希
    std::string calculateMD5(const std::string& filePath);
}

#endif // FILE_UTILS_H
