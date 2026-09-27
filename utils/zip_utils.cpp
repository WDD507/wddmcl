#include "zip_utils.h"

#include <iostream>
#include <fstream>
#include <filesystem>

extern "C" {
#include "../third_party/zlib/zlib-1.3.2/contrib/minizip/unzip.h"
}

namespace fs = std::filesystem;

namespace ZipUtils {

// 判断字符串 s 是否以 prefix 开头
static bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() &&
           s.compare(0, prefix.size(), prefix) == 0;
}

// 把 zip 内条目名拼到 destDir 下，并做路径逃逸防护（拒绝含 ".." 的条目）
static bool safeJoinPath(const fs::path& destDir,
                         const std::string& entryName,
                         fs::path& outPath) {
    fs::path rel = fs::path(entryName).lexically_normal();
    if (rel.is_absolute() || rel.string().find("..") != std::string::npos) {
        std::cerr << "[ZipUtils] 跳过不安全路径: " << entryName << std::endl;
        return false;
    }
    outPath = destDir / rel;
    return true;
}

std::vector<std::string> listFiles(const std::string& zipFile) {
    std::vector<std::string> result;
    unzFile uf = unzOpen(zipFile.c_str());
    if (!uf) {
        std::cerr << "[ZipUtils] 无法打开 ZIP: " << zipFile << std::endl;
        return result;
    }

    char nameBuf[512];
    unz_file_info info;
    if (unzGoToFirstFile(uf) == UNZ_OK) {
        do {
            if (unzGetCurrentFileInfo(uf, &info, nameBuf, sizeof(nameBuf),
                                      nullptr, 0, nullptr, 0) == UNZ_OK) {
                result.emplace_back(nameBuf);
            }
        } while (unzGoToNextFile(uf) == UNZ_OK);
    }

    unzClose(uf);
    return result;
}

// 实际解压逻辑；excludePrefix 为 nullptr 表示不排除
static bool doExtract(unzFile uf, const std::string& destDir,
                      const std::string* excludePrefix) {
    fs::path dest = fs::path(destDir);
    std::error_code ec;
    fs::create_directories(dest, ec);

    char nameBuf[512];
    unz_file_info info;

    if (unzGoToFirstFile(uf) != UNZ_OK) {
        return false;
    }

    do {
        if (unzGetCurrentFileInfo(uf, &info, nameBuf, sizeof(nameBuf),
                                  nullptr, 0, nullptr, 0) != UNZ_OK) {
            continue;
        }

        std::string entryName(nameBuf);

        // 按前缀排除（如 META-INF/）
        if (excludePrefix && startsWith(entryName, *excludePrefix)) {
            continue;
        }

        // 目录条目：以 '/' 结尾，只创建目录
        if (!entryName.empty() && entryName.back() == '/') {
            fs::path dirPath;
            if (safeJoinPath(dest, entryName, dirPath)) {
                fs::create_directories(dirPath, ec);
            }
            continue;
        }

        // 普通文件：先确保父目录存在
        fs::path outPath;
        if (!safeJoinPath(dest, entryName, outPath)) {
            continue;
        }
        fs::create_directories(outPath.parent_path(), ec);

        if (unzOpenCurrentFile(uf) != UNZ_OK) {
            continue;
        }

        std::ofstream out(outPath, std::ios::binary);
        if (!out) {
            unzCloseCurrentFile(uf);
            std::cerr << "[ZipUtils] 无法创建输出文件: " << outPath.string() << std::endl;
            continue;
        }

        char buf[8192];
        int read = 0;
        bool ok = true;
        while ((read = unzReadCurrentFile(uf, buf, sizeof(buf))) > 0) {
            out.write(buf, read);
        }
        if (read < 0) {
            ok = false;
        }
        out.close();
        unzCloseCurrentFile(uf);

        if (!ok) {
            std::cerr << "[ZipUtils] 读取条目失败: " << entryName << std::endl;
        }
    } while (unzGoToNextFile(uf) == UNZ_OK);

    return true;
}

bool extractAll(const std::string& zipFile, const std::string& destDir) {
    unzFile uf = unzOpen(zipFile.c_str());
    if (!uf) {
        std::cerr << "[ZipUtils] 无法打开 ZIP: " << zipFile << std::endl;
        return false;
    }
    bool ok = doExtract(uf, destDir, nullptr);
    unzClose(uf);
    return ok;
}

bool extractAllExclude(const std::string& zipFile, const std::string& destDir,
                       const std::string& excludePrefix) {
    unzFile uf = unzOpen(zipFile.c_str());
    if (!uf) {
        std::cerr << "[ZipUtils] 无法打开 ZIP: " << zipFile << std::endl;
        return false;
    }
    bool ok = doExtract(uf, destDir, &excludePrefix);
    unzClose(uf);
    return ok;
}

bool extractFileToBuffer(const std::string& zipFile,
                         const std::string& innerPath,
                         std::vector<unsigned char>& outBuffer) {
    unzFile uf = unzOpen(zipFile.c_str());
    if (!uf) {
        std::cerr << "[ZipUtils] 无法打开 ZIP: " << zipFile << std::endl;
        return false;
    }

    bool ok = false;
    // iCaseSensitivity = 1 表示大小写敏感
    if (unzLocateFile(uf, innerPath.c_str(), 1) == UNZ_OK) {
        if (unzOpenCurrentFile(uf) == UNZ_OK) {
            char buf[8192];
            int read = 0;
            while ((read = unzReadCurrentFile(uf, buf, sizeof(buf))) > 0) {
                outBuffer.insert(outBuffer.end(), buf, buf + read);
            }
            ok = (read >= 0);
            unzCloseCurrentFile(uf);
        }
    } else {
        std::cerr << "[ZipUtils] ZIP 内未找到条目: " << innerPath << std::endl;
    }

    unzClose(uf);
    return ok;
}

} // namespace ZipUtils
