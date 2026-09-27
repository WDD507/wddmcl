#include "file_system_utils.h"
//#include "../app.h"
#include "../config/game_config.h"
#if defined(_HAS_CXX17) || defined(_MSC_VER)
#undef _HAS_CXX17 
#define _HAS_CXX17 1
#endif
//#endif
#include <sstream>
#include <string>
#if __cplusplus < 201703L
#ifdef _MSC_VER
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif
#else
#include <filesystem>
#endif
namespace fs = std::filesystem;


std::string FileSystemUtils::getSlash() {
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

// ==================== 目录操作 ====================

bool FileSystemUtils::ensureDirectoryExists(const std::string& path) {
    try {
        return fs::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 创建目录失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystemUtils::directoryExists(const std::string& path) {
    try {
        return fs::exists(path) && fs::is_directory(path);
    } catch (...) {
        return false;
    }
}

bool FileSystemUtils::deleteDirectory(const std::string& path) {
    try {
        return fs::remove_all(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 删除目录失败: " << e.what() << std::endl;
        return false;
    }
}

// ==================== 文件操作 ====================

bool FileSystemUtils::fileExists(const std::string& path) {
    try {
        return fs::exists(path) && fs::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

std::string FileSystemUtils::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystemUtils::writeFile(const std::string& path, const std::string& content) {
    try {
        // 确保父目录存在
        fs::path filePath(path);
        if (filePath.has_parent_path()) {
            ensureDirectoryExists(filePath.parent_path().string());
        }
        
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 写入文件失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystemUtils::appendToFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 追加文件失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystemUtils::copyFile(const std::string& source, const std::string& dest) {
    try {
        fs::copy_file(source, dest, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 复制文件失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystemUtils::moveFile(const std::string& source, const std::string& dest) {
    try {
        fs::rename(source, dest);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 移动文件失败: " << e.what() << std::endl;
        return false;
    }
}

bool FileSystemUtils::deleteFile(const std::string& path) {
    try {
        return fs::remove(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 删除文件失败: " << e.what() << std::endl;
        return false;
    }
}

long long FileSystemUtils::getFileSize(const std::string& path) {
    try {
        return static_cast<long long>(fs::file_size(path));
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 获取文件大小失败: " << e.what() << std::endl;
        return -1;
    }
}

// ==================== 目录遍历 ====================

std::vector<std::string> FileSystemUtils::listFiles(const std::string& directory, 
                                                     const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        if (!directoryExists(directory)) {
            return files;
        }
        
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // 如果指定了扩展名，只返回匹配的文件
                if (extension.empty() || 
                    entry.path().extension().string() == extension) {
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 列出文件失败: " << e.what() << std::endl;
    }
    
    return files;
}

std::vector<std::string> FileSystemUtils::listFilesRecursive(const std::string& directory,
                                                              const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        if (!directoryExists(directory)) {
            return files;
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                
                // 如果指定了扩展名，只返回匹配的文件
                if (extension.empty() || 
                    entry.path().extension().string() == extension) {
                    files.push_back(filepath);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 递归列出文件失败: " << e.what() << std::endl;
    }
    
    return files;
}

// ==================== 路径处理 ====================

std::string FileSystemUtils::buildPath(const std::string& base, const std::string& subpath) {
    fs::path result = fs::path(base) / subpath;
    return result.string();
}

std::string FileSystemUtils::getParentDirectory(const std::string& path) {
    fs::path p(path);
    return p.parent_path().string();
}

std::string FileSystemUtils::getFileName(const std::string& path) {
    fs::path p(path);
    return p.filename().string();
}

std::string FileSystemUtils::getFileExtension(const std::string& path) {
    fs::path p(path);
    return p.extension().string();
}

std::string FileSystemUtils::getFileNameWithoutExtension(const std::string& path) {
    fs::path p(path);
    return p.stem().string();
}

// ==================== Minecraft 特定功能 ====================

std::string FileSystemUtils::getMinecraftDirectory() {
    // 优先从 game_config.json 解析当前激活目录
    return GameConfig::instance().getMinecraftDirectory();
}
std::string (*FileSystemUtils::getMCDir)() =
    FileSystemUtils::getMinecraftDirectory;
/* 旧代码（user 二选一硬编码版本）
std::string FileSystemUtils::getMinecraftDirectory(bool User) {
    return (User ? getUserMinecraftDirectory() : getAppMinecraftDirectory());
}
std::string (*FileSystemUtils::getMCDir)(bool)=
    FileSystemUtils::getMinecraftDirectory;
*/

std::string FileSystemUtils::getUserMinecraftDirectory() {
#ifdef _WIN32
    // Windows: %APPDATA%\.minecraft
    const char* appData = std::getenv("APPDATA");
    if (appData) {
        return std::string(appData) + "\\.minecraft";
    }
    return "C:\\Users\\Default\\AppData\\Roaming\\.minecraft";
    
#elif __APPLE__
    // macOS: ~/Library/Application Support/minecraft
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Application Support/minecraft";
    }
    return "/Users/default/Library/Application Support/minecraft";
    
#elif __linux__
    // Linux: ~/.minecraft
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.minecraft";
    }
    return "/home/default/.minecraft";
    
#else
    return ".minecraft";
#endif
}
std::string FileSystemUtils::getAppMinecraftDirectory() {
#ifdef _WIN32
    return ".\\.minecraft";
#else
    return "./.minecraft";
#endif
}
/* 旧代码（已移至文件上方，去 user 参数）
std::string FileSystemUtils::getMinecraftDirectory(bool User) {
    return (User ? getUserMinecraftDirectory() : getAppMinecraftDirectory());
}
std::string (*FileSystemUtils::getMCDir)(bool)=
    FileSystemUtils::getMinecraftDirectory;
*/

std::string FileSystemUtils::getVersionDirectory(
    const std::string& version
) {
    std::string mcDir = getMinecraftDirectory();
    return buildPath(
        mcDir, "versions" + getSlash() + version
    );
    /* 旧代码（带 user 参数）
    std::string mcDir = getMinecraftDirectory(user);
    return buildPath(
        mcDir, "versions" + getSlash() + version
    );
    */
}

std::string FileSystemUtils::getModsDirectory(
    const std::string& version
) {
    std::string mcDir = getMinecraftDirectory();
    if (version.empty()) {
        return buildPath(mcDir, "mods");
    }
    return buildPath(
        mcDir,
        "versions" + getSlash() + version + getSlash() + "mods"
    );
    /* 旧代码（带 user 参数）
    std::string mcDir = getMinecraftDirectory(user);
    if (version.empty()) {
        return buildPath(mcDir, "mods");
    }
    return buildPath(
        mcDir,
        "versions" + getSlash() + version + getSlash() + "mods"
    );
    */
}

std::string FileSystemUtils::getAssetsDirectory() {
    return buildPath(getMinecraftDirectory(), "assets");
    /* 旧代码（带 user 参数）
    std::string mcDir = getMinecraftDirectory(user);
    return buildPath(mcDir, "assets");
    */
}

std::string FileSystemUtils::getLibrariesDirectory() {
    return buildPath(getMinecraftDirectory(), "libraries");
    /* 旧代码（带 user 参数）
    std::string mcDir = getMinecraftDirectory(user);
    return buildPath(mcDir, "libraries");
    */
}

bool FileSystemUtils::isVersionInstalled(
    const std::string& version
) {
    std::string versionDir = getVersionDirectory(version);
    return directoryExists(versionDir);
    /* 旧代码（带 user 参数）
    std::string versionDir = getVersionDirectory(version, user);
    return directoryExists(versionDir);
    */
}

std::vector<std::string> FileSystemUtils::getInstalledVersions() {
    std::vector<std::string> versions;

    std::string versionsDir = buildPath(
        getMinecraftDirectory(), "versions"
    );

    if (!directoryExists(versionsDir)) {
        return versions;
    }

    try {
        for (const auto& entry : fs::directory_iterator(versionsDir)) {
            if (entry.is_directory()) {
                versions.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 获取版本列表失败: " << e.what() << std::endl;
    }

    return versions;
    /* 旧代码（带 user 参数）
    std::string versionsDir = buildPath(
        getMinecraftDirectory(user), "versions"
    );
    ...
    */
}

std::vector<std::string> FileSystemUtils::getInstalledMods(
    const std::string &version
) {
    std::vector<std::string> mods;

    std::string modsDir = buildPath(
        getVersionDirectory(version), "mods"
    );

    if (!directoryExists(modsDir)) {
        return mods;
    }

    try {
        for (const auto& entry : fs::directory_iterator(modsDir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".jar") {
                    mods.push_back(entry.path().filename().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FileSystemUtils] 获取Mod列表失败: " << e.what() << std::endl;
    }

    return mods;
    /* 旧代码（带 user 参数）
    std::string modsDir = buildPath(
        getVersionDirectory(version, user), "mods"
    );
    ...
    */
}
