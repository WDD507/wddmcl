# FileSystemUtils 使用指南

## ✅ 已完成

文件系统工具类已经成功创建并可以使用！

- 📁 头文件：`utils/file_system_utils.h`
- 📄 实现文件：`utils/file_system_utils.cpp`
- ✨ 特性：完全跨平台（Windows、Linux、macOS）

---

## 🚀 快速开始

### 测试 FileSystemUtils

运行测试程序：

```bash
# 方法1：使用批处理脚本
.\test_filesystem.bat

# 方法2：手动编译
g++.exe -I. -I./third_party -std=c++17 test_filesystem.cpp utils/file_system_utils.cpp -o test_filesystem.exe
.\test_filesystem.exe
```

---

## 📖 基本用法

### 引入头文件

```cpp
#include "utils/file_system_utils.h"
```

### 目录操作

```cpp
// 创建目录（如果不存在）
FileSystemUtils::ensureDirectoryExists("data/config");

// 检查目录是否存在
if (FileSystemUtils::directoryExists("data")) {
    std::cout << "目录存在" << std::endl;
}

// 删除目录（递归删除所有内容）
FileSystemUtils::deleteDirectory("temp_folder");
```

### 文件读写

```cpp
// 写入文件
FileSystemUtils::writeFile("config.json", "{\"key\": \"value\"}");

// 读取文件
try {
    std::string content = FileSystemUtils::readFile("config.json");
    std::cout << content << std::endl;
} catch (const std::exception& e) {
    std::cerr << "错误: " << e.what() << std::endl;
}

// 追加到文件
FileSystemUtils::appendToFile("log.txt", "\n新的日志行");
```

### 文件操作

```cpp
// 检查文件是否存在
if (FileSystemUtils::fileExists("file.txt")) {
    std::cout << "文件存在" << std::endl;
}

// 复制文件
FileSystemUtils::copyFile("source.txt", "dest.txt");

// 移动/重命名文件
FileSystemUtils::moveFile("old_name.txt", "new_name.txt");

// 删除文件
FileSystemUtils::deleteFile("unwanted.txt");

// 获取文件大小
long long size = FileSystemUtils::getFileSize("large_file.bin");
std::cout << "大小: " << size << " bytes" << std::endl;
```

### 目录遍历

```cpp
// 列出目录中的所有文件
auto files = FileSystemUtils::listFiles("mods", ".jar");
for (const auto& file : files) {
    std::cout << "Mod: " << file << std::endl;
}

// 递归列出所有文件
auto allFiles = FileSystemUtils::listFilesRecursive("assets");
```

### 路径处理

```cpp
// 构建路径（自动处理分隔符）
std::string path = FileSystemUtils::buildPath("folder", "subfolder/file.txt");
// Windows: folder\subfolder\file.txt
// Linux: folder/subfolder/file.txt

// 获取文件名
std::string filename = FileSystemUtils::getFileName("/path/to/file.txt");
// 输出: file.txt

// 获取扩展名
std::string ext = FileSystemUtils::getFileExtension("file.txt");
// 输出: .txt

// 获取不含扩展名的文件名
std::string name = FileSystemUtils::getFileNameWithoutExtension("file.txt");
// 输出: file

// 获取父目录
std::string parent = FileSystemUtils::getParentDirectory("/path/to/file.txt");
// 输出: /path/to
```

---

## 🎮 Minecraft 启动器专用功能

### 获取 Minecraft 目录

```cpp
// 自动检测操作系统，返回正确的Minecraft目录
std::string mcDir = FileSystemUtils::getMinecraftDirectory();

// Windows: C:\Users\用户名\AppData\Roaming\.minecraft
// macOS: /Users/用户名/Library/Application Support/minecraft
// Linux: /home/用户名/.minecraft

std::cout << "Minecraft目录: " << mcDir << std::endl;
```

### 版本管理

```cpp
// 检查版本是否已安装
if (FileSystemUtils::isVersionInstalled("1.20.1")) {
    std::cout << "版本 1.20.1 已安装" << std::endl;
}

// 获取所有已安装的版本
auto versions = FileSystemUtils::getInstalledVersions();
std::cout << "已安装 " << versions.size() << " 个版本:" << std::endl;
for (const auto& version : versions) {
    std::cout << "  - " << version << std::endl;
}

// 获取版本目录路径
std::string versionDir = FileSystemUtils::getVersionDirectory("1.20.1");
// 输出: C:\...\AppData\Roaming\.minecraft\versions\1.20.1
```

### Mod 管理

```cpp
// 获取主 Mod 目录
std::string modsDir = FileSystemUtils::getModsDirectory();
// 输出: C:\...\AppData\Roaming\.minecraft\mods

// 获取特定版本的 Mod 目录
std::string versionModsDir = FileSystemUtils::getModsDirectory("1.20.1");
// 输出: C:\...\AppData\Roaming\.minecraft\versions\1.20.1\mods

// 列出所有 Mod
auto mods = FileSystemUtils::listFiles(modsDir, ".jar");
for (const auto& mod : mods) {
    std::cout << "Mod: " << mod << std::endl;
}
```

### 其他目录

```cpp
// 资源目录
std::string assetsDir = FileSystemUtils::getAssetsDirectory();

// 库目录
std::string libsDir = FileSystemUtils::getLibrariesDirectory();
```

---

## 💡 实际应用示例

### 示例1：读取版本配置文件

```cpp
#include "utils/file_system_utils.h"
#include "utils/json_parser.h"

void loadVersionConfig(const std::string& version) {
    std::string versionDir = FileSystemUtils::getVersionDirectory(version);
    std::string configFile = FileSystemUtils::buildPath(versionDir, "version.json");
    
    if (!FileSystemUtils::fileExists(configFile)) {
        std::cerr << "版本配置文件不存在: " << configFile << std::endl;
        return;
    }
    
    try {
        // 读取JSON文件
        json config = JsonParser::parseFile(configFile);
        
        // 解析版本信息
        std::string id = config["id"].get<std::string>();
        std::string mainClass = config["mainClass"].get<std::string>();
        
        std::cout << "版本ID: " << id << std::endl;
        std::cout << "主类: " << mainClass << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "解析配置失败: " << e.what() << std::endl;
    }
}
```

### 示例2：扫描并加载Mod列表

```cpp
void scanMods(const std::string& version) {
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    
    if (!FileSystemUtils::directoryExists(modsDir)) {
        std::cout << "Mod目录不存在，创建中..." << std::endl;
        FileSystemUtils::ensureDirectoryExists(modsDir);
        return;
    }
    
    // 获取所有.jar文件
    auto modFiles = FileSystemUtils::listFiles(modsDir, ".jar");
    
    std::cout << "找到 " << modFiles.size() << " 个Mod:" << std::endl;
    for (const auto& modFile : modFiles) {
        std::string fullPath = FileSystemUtils::buildPath(modsDir, modFile);
        long long size = FileSystemUtils::getFileSize(fullPath);
        
        std::cout << "  - " << modFile 
                  << " (" << size / 1024 << " KB)" << std::endl;
    }
}
```

### 示例3：备份配置文件

```cpp
bool backupConfig(const std::string& configPath) {
    if (!FileSystemUtils::fileExists(configPath)) {
        return false;
    }
    
    // 生成备份文件名（添加时间戳）
    std::string backupPath = configPath + ".bak";
    
    // 复制文件
    return FileSystemUtils::copyFile(configPath, backupPath);
}
```

### 示例4：清理临时文件

```cpp
void cleanupTempFiles() {
    std::string tempDir = FileSystemUtils::buildPath(
        FileSystemUtils::getMinecraftDirectory(), 
        "temp"
    );
    
    if (FileSystemUtils::directoryExists(tempDir)) {
        std::cout << "清理临时文件..." << std::endl;
        FileSystemUtils::deleteDirectory(tempDir);
        FileSystemUtils::ensureDirectoryExists(tempDir);
        std::cout << "清理完成" << std::endl;
    }
}
```

---

## ⚠️ 注意事项

1. **异常处理**: `readFile()` 可能抛出异常，务必使用 try-catch
2. **路径分隔符**: 使用 `buildPath()` 而不是手动拼接，确保跨平台兼容
3. **权限问题**: 某些目录可能需要管理员权限才能写入
4. **相对路径**: 建议使用绝对路径，避免工作目录变化导致的问题

---

## 🔧 编译要求

- **C++标准**: C++17 或更高
- **编译器**: 支持 `<filesystem>` 的编译器
  - MinGW (g++) 7.1+
  - MSVC (Visual Studio 2017+)
  - Clang 5+

**编译命令：**
```bash
g++.exe -std=c++17 main.cpp utils/file_system_utils.cpp -o main.exe
```

---

## 📚 API 参考

### 目录操作
- `ensureDirectoryExists(path)` - 确保目录存在
- `directoryExists(path)` - 检查目录是否存在
- `deleteDirectory(path)` - 删除目录

### 文件操作
- `fileExists(path)` - 检查文件是否存在
- `readFile(path)` - 读取文件内容
- `writeFile(path, content)` - 写入文件
- `appendToFile(path, content)` - 追加到文件
- `copyFile(source, dest)` - 复制文件
- `moveFile(source, dest)` - 移动文件
- `deleteFile(path)` - 删除文件
- `getFileSize(path)` - 获取文件大小

### 目录遍历
- `listFiles(directory, extension)` - 列出文件
- `listFilesRecursive(directory, extension)` - 递归列出文件

### 路径处理
- `buildPath(base, subpath)` - 构建路径
- `getParentDirectory(path)` - 获取父目录
- `getFileName(path)` - 获取文件名
- `getFileExtension(path)` - 获取扩展名
- `getFileNameWithoutExtension(path)` - 获取不含扩展名的文件名

### Minecraft 功能
- `getMinecraftDirectory()` - 获取Minecraft目录
- `getVersionDirectory(version)` - 获取版本目录
- `getModsDirectory(version)` - 获取Mod目录
- `getAssetsDirectory()` - 获取资源目录
- `getLibrariesDirectory()` - 获取库目录
- `isVersionInstalled(version)` - 检查版本是否安装
- `getInstalledVersions()` - 获取已安装版本列表

---

## 🎯 下一步建议

现在你可以：

1. ✅ 在 `ConfigManager` 中使用文件系统工具保存/加载配置
2. ✅ 在 `VersionManager` 中扫描已安装的版本
3. ✅ 在 `ModManager` 中管理Mod文件
4. ✅ 实现游戏文件的下载和安装
5. ✅ 实现配置备份和恢复功能

祝你开发顺利！🚀
