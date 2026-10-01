#include "java_detector.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace fs = std::filesystem;

// ==================== 静态辅助函数 ====================

// 判断目录名是否应该跳过（系统目录/大型无关目录，加快扫描）
static bool shouldSkipDir(const fs::path& dir, const std::vector<std::string>& excludeDirs) {
    std::string name = dir.filename().string();
    // 转小写比较
    std::string nameLower = name;
    for (auto& c : nameLower) c = static_cast<char>(tolower(c));

    // 默认跳过列表：系统目录和大型无关目录
    static const std::vector<std::string> defaultSkipList = {
        "windows", "$recycle.bin", "system volume information",
        "programdata", "$windows.~ws", "$windows.~bt",
        "msocache", "recovery", "boot", "efi",
        "node_modules", ".git", "dist", "build", "target",
        "__pycache__", ".vscode", ".idea", "bin", "obj",
        "cache", ".cache", "temp", "tmp"
    };
    for (const auto& s : defaultSkipList) {
        if (nameLower == s) return true;
    }

    // 用户自定义排除目录
    for (const auto& exclude : excludeDirs) {
        std::string excludeLower = exclude;
        for (auto& c : excludeLower) c = static_cast<char>(tolower(c));
        // 支持路径片段匹配：如果排除目录是完整路径的前缀，或目录名匹配
        std::string dirStr = dir.string();
        std::string dirStrLower = dirStr;
        for (auto& c : dirStrLower) c = static_cast<char>(tolower(c));
        // 完整路径前缀匹配（如 "C:\Games" 排除整个 Games 目录）
        if (dirStrLower.find(excludeLower) != std::string::npos) return true;
        // 目录名精确匹配
        if (nameLower == excludeLower) return true;
    }

    return false;
}

// 递归查找目录下的 java.exe（带跳过逻辑和深度限制）
static void findJavaInDir(
    const fs::path& dir,
    std::vector<std::string>& results,
    const std::vector<std::string>& excludeDirs,
    int maxDepth,
    int depth = 0
) {
    if (depth > maxDepth) return;
    try {
        for (const auto& entry : fs::directory_iterator(
            dir, fs::directory_options::skip_permission_denied
        )) {
            try {
                if (entry.is_directory()) {
                    if (!shouldSkipDir(entry.path(), excludeDirs)) {
                        findJavaInDir(entry.path(), results, excludeDirs,
                                     maxDepth, depth + 1);
                    }
                } else if (entry.is_regular_file()) {
                    std::string fname = entry.path().filename().string();
                    // 转小写比较
                    std::string fnameLower = fname;
                    for (auto& c : fnameLower)
                        c = static_cast<char>(tolower(c));
#ifdef _WIN32
                    // Windows: 找 java.exe 且在 bin 目录下
                    if (fnameLower == "java.exe" &&
                        entry.path().parent_path().filename() == "bin") {
                        results.push_back(entry.path().string());
                    }
#else
                    // Linux/macOS: 找 java 可执行文件
                    if (fname == "java" &&
                        entry.path().parent_path().filename() == "bin") {
                        results.push_back(entry.path().string());
                    }
#endif
                }
            } catch (...) {
                continue;
            }
        }
    } catch (...) {
        // 权限不足等，跳过
    }
}

// ==================== 私有辅助方法 ====================

std::string JavaDetector::execCommand(const std::string& command) {
    std::string result;
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (pipe) {
        char buffer[512];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }
    return result;
}

int JavaDetector::detectVersionFromPath(const std::string& path) {
    // 从路径中提取版本号（例如：jdk-17.0.1 -> 17）
    try {
        size_t pos = path.find("jdk-");
        if (pos != std::string::npos) {
            std::string versionStr = path.substr(pos + 4);
            size_t dotPos = versionStr.find('.');
            if (dotPos != std::string::npos) {
                return std::stoi(versionStr.substr(0, dotPos));
            }
        }
    } catch (...) {
        // 解析失败
    }
    return 0;
}

std::string JavaDetector::extractVendor(const std::string& path) {
    // 从路径中提取供应商名称
    std::string pathLower = path;
    for (auto& c : pathLower) c = static_cast<char>(tolower(c));

    if (pathLower.find("microsoft") != std::string::npos) {
        return "Microsoft";
    } else if (pathLower.find("adoptopenjdk") != std::string::npos ||
               pathLower.find("adoptium") != std::string::npos ||
               pathLower.find("temurin") != std::string::npos) {
        return "Adoptium";
    } else if (pathLower.find("oracle") != std::string::npos) {
        return "Oracle";
    } else if (pathLower.find("zulu") != std::string::npos) {
        return "Azul Zulu";
    } else if (pathLower.find("bellsoft") != std::string::npos ||
               pathLower.find("liberica") != std::string::npos) {
        return "Liberica";
    } else if (pathLower.find("corretto") != std::string::npos) {
        return "Amazon Corretto";
    }
    return "Unknown";
}

int JavaDetector::parseVersionFromString(const std::string& output) {
    // 从 java -version 输出中解析版本号
    // 支持两种格式：
    //   旧版：java version "1.8.0_202"  → 返回 8
    //   新版：openjdk version "17.0.1" → 返回 17
    try {
        size_t quoteStart = output.find('"');
        if (quoteStart == std::string::npos) return 0;
        size_t quoteEnd = output.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos) return 0;

        std::string versionStr = output.substr(
            quoteStart + 1, quoteEnd - quoteStart - 1
        );

        // 旧版本格式：1.8.0_202 → 取第二段 8
        if (versionStr.rfind("1.", 0) == 0) {
            size_t firstDot = versionStr.find('.');
            if (firstDot != std::string::npos) {
                size_t secondDot = versionStr.find('.', firstDot + 1);
                std::string major = (secondDot != std::string::npos)
                    ? versionStr.substr(firstDot + 1, secondDot - firstDot - 1)
                    : versionStr.substr(firstDot + 1);
                // 去掉可能的下划线后缀（如 0_202）
                size_t underscore = major.find('_');
                if (underscore != std::string::npos) {
                    major = major.substr(0, underscore);
                }
                return std::stoi(major);
            }
        }

        // 新版本格式：17.0.1 → 取第一段 17
        size_t dotPos = versionStr.find('.');
        if (dotPos != std::string::npos) {
            return std::stoi(versionStr.substr(0, dotPos));
        }
        return std::stoi(versionStr);
    } catch (...) {
        // 解析失败
    }
    return 0;
}

// ==================== 公有方法 ====================

std::vector<JavaInfo> JavaDetector::detectJavaInstallations(
    const JavaSearchOptions& options
) {
    std::cout << "[JavaDetector] Detecting Java installations..." << std::endl;

    std::vector<JavaInfo> javaList;
    std::set<std::string> foundPaths;  // 去重

#ifdef _WIN32
    // 1. JAVA_HOME（快速）
    const char* javaHome = std::getenv("JAVA_HOME");
    if (javaHome) {
        std::string path = std::string(javaHome) + "\\bin\\java.exe";
        if (fs::exists(path)) {
            foundPaths.insert(path);
            std::cout << "[JavaDetector] Found (JAVA_HOME): " << path << std::endl;
        }
    }

    // 2. 系统 PATH 中的 java（快速）
    std::string whereOutput = execCommand("where java 2>nul");
    {
        size_t start = 0, end;
        while ((end = whereOutput.find('\n', start)) != std::string::npos) {
            std::string p = whereOutput.substr(start, end - start);
            // 去掉回车
            while (!p.empty() && (p.back() == '\r' || p.back() == '\n'))
                p.pop_back();
            if (!p.empty() && fs::exists(p)) {
                foundPaths.insert(p);
                std::cout << "[JavaDetector] Found (PATH): " << p << std::endl;
            }
            start = end + 1;
        }
        // 最后一段
        std::string p = whereOutput.substr(start);
        while (!p.empty() && (p.back() == '\r' || p.back() == '\n'))
            p.pop_back();
        if (!p.empty() && fs::exists(p)) {
            foundPaths.insert(p);
            std::cout << "[JavaDetector] Found (PATH): " << p << std::endl;
        }
    }

    // 3. 确定扫描范围
    if (!options.includeDirs.empty()) {
        // 只扫描用户指定的目录
        std::cout << "[JavaDetector] 只扫描指定目录：" << std::endl;
        for (const auto& dir : options.includeDirs) {
            std::cout << "  - " << dir << std::endl;
            std::vector<std::string> dirResults;
            findJavaInDir(dir, dirResults, options.excludeDirs, options.maxDepth);
            for (const auto& p : dirResults) {
                foundPaths.insert(p);
            }
        }
    } else {
        // 全盘扫描所有分区
        std::cout << "[JavaDetector] 全盘扫描所有分区..." << std::endl;
        DWORD drives = GetLogicalDrives();
        for (char c = 'A'; c <= 'Z'; c++) {
            if (!(drives & (1 << (c - 'A')))) continue;
            std::string drive = std::string(1, c) + ":\\";
            UINT type = GetDriveTypeA(drive.c_str());
            // 只扫描固定盘和可移动盘，跳过光驱/网络盘
            if (type != DRIVE_FIXED && type != DRIVE_REMOVABLE) continue;

            std::cout << "[JavaDetector] 扫描分区 " << drive << "..." << std::endl;
            std::vector<std::string> driveResults;
            findJavaInDir(drive, driveResults, options.excludeDirs,
                          options.maxDepth);
            for (const auto& p : driveResults) {
                foundPaths.insert(p);
            }
        }
    }

#else
    // Linux/macOS
    const char* javaHome = std::getenv("JAVA_HOME");
    if (javaHome) {
        std::string path = std::string(javaHome) + "/bin/java";
        if (fs::exists(path)) {
            foundPaths.insert(path);
        }
    }

    if (!options.includeDirs.empty()) {
        for (const auto& dir : options.includeDirs) {
            std::vector<std::string> dirResults;
            findJavaInDir(dir, dirResults, options.excludeDirs, options.maxDepth);
            for (const auto& p : dirResults) {
                foundPaths.insert(p);
            }
        }
    } else {
        // 扫描常见位置
        std::vector<std::string> searchPaths = {
            "/usr/lib/jvm",
            "/usr/local/lib",
            "/opt",
            "/Library/Java/JavaVirtualMachines"
        };
        for (const auto& path : searchPaths) {
            if (!fs::exists(path)) continue;
            std::vector<std::string> dirResults;
            findJavaInDir(path, dirResults, options.excludeDirs,
                          options.maxDepth);
            for (const auto& p : dirResults) {
                foundPaths.insert(p);
            }
        }
    }
#endif

    // 4. 解析每个找到的 java 的版本
    for (const auto& path : foundPaths) {
        JavaInfo info;
        info.path = path;
        // 执行 java -version 获取真实版本
        std::string output = execCommand("\"" + path + "\" -version 2>&1");
        info.version = parseVersionFromString(output);
        info.vendor = extractVendor(path);
        info.isValid = (info.version > 0);
        javaList.push_back(info);
        std::cout << "[JavaDetector] Found: " << path
                  << " v" << info.version
                  << " (" << info.vendor << ")"
                  << (info.isValid ? " [valid]" : " [invalid]")
                  << std::endl;
    }

    std::cout << "[JavaDetector] Total found: " << javaList.size() << std::endl;
    return javaList;
}

bool JavaDetector::validateJavaVersion(const std::string& javaPath,
                                        int requiredVersion) {
    std::cout << "[JavaDetector] Validating Java version for: "
              << javaPath << std::endl;

    if (!fs::exists(javaPath)) {
        std::cerr << "[JavaDetector] Java executable not found: "
                  << javaPath << std::endl;
        return false;
    }

    // 执行 java -version 获取版本信息
    std::string command = "\"" + javaPath + "\" -version 2>&1";
    std::string output = execCommand(command);

    if (output.empty()) {
        std::cerr << "[JavaDetector] Failed to execute java -version"
                  << std::endl;
        return false;
    }

    // 解析版本号
    int detectedVersion = parseVersionFromString(output);

    std::cout << "[JavaDetector] Detected Java version: "
              << detectedVersion << std::endl;
    std::cout << "[JavaDetector] Required version: "
              << requiredVersion << std::endl;

    bool isValid = detectedVersion >= requiredVersion;

    if (isValid) {
        std::cout << "[JavaDetector] Java version is compatible" << std::endl;
    } else {
        std::cerr << "[JavaDetector] Java version is too old" << std::endl;
    }

    return isValid;
}

std::string JavaDetector::getRecommendedJavaPath(
    int minecraftVersion,
    const JavaSearchOptions& options
) {
    std::cout << "[JavaDetector] Getting recommended Java for MC "
              << minecraftVersion << std::endl;

    // 根据Minecraft版本确定所需的Java版本
    int requiredJavaVersion = 0;

    if (minecraftVersion >= 120) {  // 1.20+
        requiredJavaVersion = 17;
    } else if (minecraftVersion >= 118) {  // 1.18-1.19
        requiredJavaVersion = 17;
    } else if (minecraftVersion >= 117) {  // 1.17
        requiredJavaVersion = 16;
    } else {  // 1.16及更早
        requiredJavaVersion = 8;
    }

    std::cout << "[JavaDetector] Required Java version: "
              << requiredJavaVersion << std::endl;

    // 扫描已安装的Java
    auto javaList = detectJavaInstallations(options);

    // 找到第一个符合要求的Java
    for (const auto& java : javaList) {
        if (java.version >= requiredJavaVersion && java.isValid) {
            std::cout << "[JavaDetector] Recommended: " << java.path
                      << std::endl;
            return java.path;
        }
    }

    // 如果没有找到，返回空字符串
    std::cerr << "[JavaDetector] No suitable Java found. Please install Java "
              << requiredJavaVersion << " or higher." << std::endl;
    return "";
}

bool JavaDetector::downloadJava(int version, const std::string& installPath) {
    std::cout << "[JavaDetector] Downloading Java " << version << " to "
        << installPath << std::endl;

    // TODO: 需要HTTP客户端下载Java
    // 可以从以下来源下载：
    // 1. Adoptium (https://adoptium.net/)
    // 2. Microsoft Build of OpenJDK
    // 3. Oracle JDK

    std::cout << "[JavaDetector] WARNING: HTTP functionality not implemented yet"
        << std::endl;
    std::cout << "[JavaDetector] Please manually download and install Java "
        << version << std::endl;
    std::cout << "[JavaDetector] Recommended sources:" << std::endl;
    std::cout << "[JavaDetector]   - Adoptium: https://adoptium.net/" << std::endl;
    std::cout << "[JavaDetector]   - Microsoft: https://www.microsoft.com/openjdk"
        << std::endl;

    return false;  // 暂时返回false
}
