#ifndef ZIP_UTILS_H
#define ZIP_UTILS_H

#include <string>
#include <vector>

// ZipUtils：基于 minizip（zlib contrib）的 ZIP 解压封装
// 依赖：third_party/zlib/zlib-1.3.2/contrib/minizip 的 unzip.c/ioapi.c
//      以及 zlib 核心（adler32/crc32/deflate/inflate/trees/zutil 等）
namespace ZipUtils {

    // 列出 ZIP 内所有条目路径（含目录）
    std::vector<std::string> listFiles(const std::string& zipFile);

    // 解压全部文件到 destDir；失败返回 false
    bool extractAll(const std::string& zipFile, const std::string& destDir);

    // 解压全部，但跳过以 excludePrefix 开头的条目（例如 "META-INF/"）
    bool extractAllExclude(const std::string& zipFile, const std::string& destDir,
                           const std::string& excludePrefix);

    // 将单个文件解压到内存缓冲区；找不到或失败返回 false
    bool extractFileToBuffer(const std::string& zipFile,
                             const std::string& innerPath,
                             std::vector<unsigned char>& outBuffer);
    
    // 将单个文件解压到另一文件
    bool extractFile(const std::string &zipFile,
                     const std::string &innerPath,
                     const std::string &outputFile);

} // namespace ZipUtils

#endif // ZIP_UTILS_H
