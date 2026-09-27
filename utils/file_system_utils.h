#ifndef FILE_SYSTEM_UTILS_H
#define FILE_SYSTEM_UTILS_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

/*#ifdef _MSC_VER
namespace fs = std::experimental::filesystem;
#elif defined(__GNUC__)
namespace fs = std::filesystem;
#else
namespace fs {
    //
};
#endif*/
/**
 * 文件系统工具类
 * 提供跨平台的文件操作功能
 */
class FileSystemUtils {
public:
    FileSystemUtils() = delete;
    FileSystemUtils(const FileSystemUtils &) = delete;
    FileSystemUtils(FileSystemUtils &&) = delete;
    FileSystemUtils &operator=(const FileSystemUtils &) = delete;
    FileSystemUtils &operator=(FileSystemUtils &&) = delete;
    ~FileSystemUtils() = delete;

    static std::string getSlash();

    // ==================== 目录操作 ====================
    
    /**
     * 确保目录存在（不存在则创建）
     * @param path 目录路径
     * @return 是否成功
     */
    static bool ensureDirectoryExists(const std::string& path);
    
    /**
     * 检查目录是否存在
     * @param path 目录路径
     * @return 是否存在
     */
    static bool directoryExists(const std::string& path);
    
    /**
     * 删除目录（递归删除所有内容）
     * @param path 目录路径
     * @return 是否成功
     */
    static bool deleteDirectory(const std::string& path);
    
    // ==================== 文件操作 ====================
    
    /**
     * 检查文件是否存在
     * @param path 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& path);
    
    /**
     * 读取文件内容为字符串
     * @param path 文件路径
     * @return 文件内容
     * @throws std::runtime_error 如果无法打开文件
     */
    static std::string readFile(const std::string& path);
    
    /**
     * 写入文件（如果父目录不存在会自动创建）
     * @param path 文件路径
     * @param content 文件内容
     * @return 是否成功
     */
    static bool writeFile(const std::string& path, const std::string& content);
    
    /**
     * 追加内容到文件末尾
     * @param path 文件路径
     * @param content 要追加的内容
     * @return 是否成功
     */
    static bool appendToFile(const std::string& path, const std::string& content);
    
    /**
     * 复制文件
     * @param source 源文件路径
     * @param dest 目标文件路径
     * @return 是否成功
     */
    static bool copyFile(const std::string& source, const std::string& dest);
    
    /**
     * 移动/重命名文件
     * @param source 源文件路径
     * @param dest 目标文件路径
     * @return 是否成功
     */
    static bool moveFile(const std::string& source, const std::string& dest);
    
    /**
     * 删除文件
     * @param path 文件路径
     * @return 是否成功
     */
    static bool deleteFile(const std::string& path);
    
    /**
     * 获取文件大小（字节）
     * @param path 文件路径
     * @return 文件大小，失败返回-1
     */
    static long long getFileSize(const std::string& path);
    
    // ==================== 目录遍历 ====================
    
    /**
     * 列出目录中的所有文件
     * @param directory 目录路径
     * @param extension 文件扩展名过滤（如".jar"，空字符串表示不过滤）
     * @return 文件名列表
     */
    static std::vector<std::string> listFiles(const std::string& directory, 
                                              const std::string& extension = "");
    
    /**
     * 递归列出目录中的所有文件
     * @param directory 目录路径
     * @param extension 文件扩展名过滤
     * @return 文件完整路径列表
     */
    static std::vector<std::string> listFilesRecursive(const std::string& directory,
                                                       const std::string& extension = "");
    
    // ==================== 路径处理 ====================
    
    /**
     * 构建路径（自动处理路径分隔符）
     * @param base 基础路径
     * @param subpath 子路径
     * @return 完整路径
     */
    static std::string buildPath(const std::string& base, const std::string& subpath);
    
    /**
     * 获取文件的父目录
     * @param path 文件路径
     * @return 父目录路径
     */
    static std::string getParentDirectory(const std::string& path);
    
    /**
     * 获取文件名（不含路径）
     * @param path 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * 获取文件扩展名
     * @param path 文件路径
     * @return 扩展名（包含点号，如".txt"）
     */
    static std::string getFileExtension(const std::string& path);
    
    /**
     * 获取不带扩展名的文件名
     * @param path 文件路径
     * @return 文件名（不含扩展名）
     */
    static std::string getFileNameWithoutExtension(const std::string& path);
    
    // ==================== Minecraft 特定功能 ====================

    /**
     * 获取Minecraft游戏目录路径
     * 优先从 game_config.json 的 directory.current 解析，
     * 找不到则 fallback 到启动器旁的 .minecraft
     * @return Minecraft目录路径
     */
    static std::string getMinecraftDirectory();
    static std::string (*getMCDir)();  // getMinecraftDirectory 的短别名

    /**
     * 获取用户系统默认 Minecraft 目录（fallback 用）
     * Windows: %APPDATA%\.minecraft
     * macOS: ~/Library/Application Support/minecraft
     * Linux: ~/.minecraft
     */
    static std::string getUserMinecraftDirectory();
    static std::string getAppMinecraftDirectory();

    /**
     * 获取指定版本的目录路径
     * @param version 版本号（如"1.20.1"）
     * @return 版本目录路径
     */
    static std::string getVersionDirectory(const std::string& version);

    /**
     * 获取Mod目录路径
     * @param version 版本号（可选，为空则返回主mods目录）
     * @return Mod目录路径
     */
    static std::string getModsDirectory(const std::string& version = "");

    /**
     * 获取资源目录路径
     * @return 资源目录路径
     */
    static std::string getAssetsDirectory();

    /**
     * 获取库目录路径
     * @return 库目录路径
     */
    static std::string getLibrariesDirectory();

    /**
     * 检查指定版本是否已安装
     * @param version 版本号
     * @return 是否已安装
     */
    static bool isVersionInstalled(const std::string& version);

    /**
     * 获取所有已安装的版本列表
     * @return 版本号列表
     */
    static std::vector<std::string> getInstalledVersions();
    static std::vector<std::string> getInstalledMods(const std::string &version);
};

#endif // FILE_SYSTEM_UTILS_H
