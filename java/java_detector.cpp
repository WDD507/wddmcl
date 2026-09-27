#include "java_detector.h"
#include <iostream>
#include <vector>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef _MSC_VER
namespace fs = std::filesystem;
#elif defined(__GNUC__)
namespace fs = std::filesystem;
#endif

std::vector<JavaInfo> JavaDetector::detectJavaInstallations() {
    std::cout << "[JavaDetector] Detecting Java installations..." << std::endl;
    
    std::vector<JavaInfo> javaList;
    
#ifdef _WIN32
    // Windows: 扫描常见的Java安装位置
    
    // 1. JAVA_HOME环境变量
    const char* javaHome = std::getenv("JAVA_HOME");
    if (javaHome) {
        std::string path = std::string(javaHome) + "\\bin\\java.exe";
        if (fs::exists(path)) {
            JavaInfo info;
            info.path = path;
            info.version = detectVersionFromPath(path);
            info.vendor = "Unknown";
            info.isValid = true;
            javaList.push_back(info);
            std::cout << "[JavaDetector] Found Java from JAVA_HOME: " << path << std::endl;
        }
    }
    
    // 2. Program Files中的Java
    std::vector<std::string> searchPaths = {
        "C:\\Program Files\\Java",
        "C:\\Program Files (x86)\\Java",
        "C:\\Program Files\\Microsoft\\jdk-*",
        "C:\\Users\\*\\AppData\\Local\\Microsoft\\jdk-*"
    };
    
    for (const auto& basePath : searchPaths) {
        try {
            if (fs::exists(basePath)) {
                for (const auto& entry : fs::directory_iterator(basePath)) {
                    if (entry.is_directory()) {
                        std::string javaPath = entry.path().string() + "\\bin\\java.exe";
                        if (fs::exists(javaPath)) {
                            JavaInfo info;
                            info.path = javaPath;
                            info.version = detectVersionFromPath(entry.path().filename().string());
                            info.vendor = extractVendor(entry.path().string());
                            info.isValid = true;
                            javaList.push_back(info);
                            std::cout << "[JavaDetector] Found Java: " << javaPath << std::endl;
                        }
                    }
                }
            }
        } catch (...) {
            // 忽略路径不存在的情况
        }
    }
    
#else
    // Linux/macOS: 扫描常见位置
    
    std::vector<std::string> searchPaths = {
        "/usr/bin/java",
        "/usr/local/bin/java",
        "/opt/java",
        "/Library/Java/JavaVirtualMachines"
    };
    
    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            JavaInfo info;
            info.path = path;
            info.version = 0;  // TODO: 检测版本
            info.vendor = "Unknown";
            info.isValid = true;
            javaList.push_back(info);
        }
    }
#endif
    
    std::cout << "[JavaDetector] Total Java installations found: " << javaList.size() << std::endl;
    return javaList;
}

bool JavaDetector::validateJavaVersion(const std::string& javaPath, int requiredVersion) {
    std::cout << "[JavaDetector] Validating Java version for: " << javaPath << std::endl;
    
    if (!fs::exists(javaPath)) {
        std::cerr << "[JavaDetector] Java executable not found: " << javaPath << std::endl;
        return false;
    }
    
    // 执行 java -version 获取版本信息
    std::string command = "\"" + javaPath + "\" -version 2>&1";
    
#ifdef _WIN32
    // Windows: 使用_popen执行命令
    FILE* pipe = _popen(command.c_str(), "r");
#else
    // Linux/macOS: 使用popen
    FILE* pipe = popen(command.c_str(), "r");
#endif
    
    if (!pipe) {
        std::cerr << "[JavaDetector] Failed to execute java -version" << std::endl;
        return false;
    }
    
    char buffer[256];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    
    // 解析版本号（例如：openjdk version "17.0.1"）
    int detectedVersion = parseVersionFromString(output);
    
    std::cout << "[JavaDetector] Detected Java version: " << detectedVersion << std::endl;
    std::cout << "[JavaDetector] Required version: " << requiredVersion << std::endl;
    
    bool isValid = detectedVersion >= requiredVersion;
    
    if (isValid) {
        std::cout << "[JavaDetector] ✓ Java version is compatible" << std::endl;
    } else {
        std::cerr << "[JavaDetector] ✗ Java version is too old" << std::endl;
    }
    
    return isValid;
}

std::string JavaDetector::getRecommendedJavaPath(int minecraftVersion) {
    std::cout << "[JavaDetector] Getting recommended Java for MC " << minecraftVersion << std::endl;
    
    // 根据Minecraft版本确定所需的Java版本
    int requiredJavaVersion=0;
    
    if (minecraftVersion >= 120) {  // 1.20+
        requiredJavaVersion = 17;
    } else if (minecraftVersion >= 118) {  // 1.18-1.19
        requiredJavaVersion = 17;
    } else if (minecraftVersion >= 117) {  // 1.17
        requiredJavaVersion = 16;
    } else {  // 1.16及更早
        requiredJavaVersion = 8;
    }
    
    std::cout << "[JavaDetector] Required Java version: " << requiredJavaVersion << std::endl;
    
    // 扫描已安装的Java
    auto javaList = detectJavaInstallations();
    
    // 找到第一个符合要求的Java
    for (const auto& java : javaList) {
        if (java.version >= requiredJavaVersion && java.isValid) {
            std::cout << "[JavaDetector] Recommended: " << java.path << std::endl;
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

// ==================== 私有辅助方法 ====================

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
    if (path.find("Microsoft") != std::string::npos) {
        return "Microsoft";
    } else if (
        path.find("AdoptOpenJDK") != std::string::npos
        || path.find("adoptium") != std::string::npos
    ) {
        return "Adoptium";
    } else if (path.find("Oracle") != std::string::npos) {
        return "Oracle";
    } else if (path.find("Zulu") != std::string::npos) {
        return "Azul Zulu";
    }
    return "Unknown";
}

int JavaDetector::parseVersionFromString(const std::string& output) {
    // 从 java -version 输出中解析版本号
    // 例如：openjdk version "17.0.1" 2021-10-19
    try {
        size_t quoteStart = output.find('"');
        if (quoteStart != std::string::npos) {
            size_t quoteEnd = output.find('"', quoteStart + 1);
            if (quoteEnd != std::string::npos) {
                std::string versionStr = output.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                size_t dotPos = versionStr.find('.');
                if (dotPos != std::string::npos) {
                    return std::stoi(versionStr.substr(0, dotPos));
                }
                return std::stoi(versionStr);
            }
        }
    } catch (...) {
        // 解析失败
    }
    return 0;
}
