# ProcessUtils 使用指南

## ✅ 已完成

进程工具类已经成功创建并可以使用！

- 📁 头文件：`utils/process_utils.h`
- 📄 实现文件：`utils/process_utils.cpp`
- ✨ 特性：完全跨平台（Windows、Linux、macOS）
- 🔧 技术：使用 `std::async` 实现非阻塞执行

---

## 🚀 快速开始

### 测试 ProcessUtils

运行测试程序：

```bash
# 方法1：使用批处理脚本
.\test_process.bat

# 方法2：手动编译
g++.exe -I. -I./third_party -std=c++17 test_process.cpp utils/process_utils.cpp utils/file_system_utils.cpp -o test_process.exe
.\test_process.exe
```

---

## 📖 基本用法

### 引入头文件

```cpp
#include "utils/process_utils.h"
```

### 1. 启动外部进程（非阻塞）

```cpp
// Windows: 打开记事本
ProcessUtils::launchProcess("notepad.exe", false);

// Linux/macOS: 打开文本编辑器
ProcessUtils::launchProcess("gedit", false);

// 立即返回，不等待进程完成
std::cout << "程序已启动，继续执行..." << std::endl;
```

### 2. 启动外部进程（阻塞）

```cpp
// 等待命令执行完成
bool success = ProcessUtils::launchProcess("ping google.com", true);

if (success) {
    std::cout << "命令执行完成" << std::endl;
}
```

### 3. 后台执行（带回调）

```cpp
// 在后台线程中执行，完成后调用回调函数
ProcessUtils::launchInBackground("dir", [](int exitCode) {
    std::cout << "命令执行完成，退出码: " << exitCode << std::endl;
    
    if (exitCode == 0) {
        std::cout << "成功！" << std::endl;
    } else {
        std::cout << "失败！" << std::endl;
    }
});

// 主程序可以继续执行其他任务
std::cout << "后台任务已启动..." << std::endl;
```

### 4. Minecraft 启动器专用

```cpp
// 启动 Minecraft 游戏
std::string javaPath = "C:/Program Files/Java/jdk-17/bin/java.exe";
std::string gameArgs = "-Xmx4G -Xms2G -jar minecraft.jar --username Player";

bool launched = ProcessUtils::launchMinecraft(javaPath, gameArgs);

if (launched) {
    std::cout << "游戏已在后台启动！" << std::endl;
} else {
    std::cerr << "启动失败！" << std::endl;
}
```

### 5. 打开文件或文件夹

```cpp
// 打开文件夹
ProcessUtils::openFileOrFolder("C:/Users");

// 打开文件
ProcessUtils::openFileOrFolder("config.json");

// 跨平台：自动选择正确的打开方式
// Windows: ShellExecute
// macOS: open
// Linux: xdg-open
```

---

## 🎮 在 Minecraft 启动器中的应用

### 示例1：启动游戏

```cpp
#include "utils/process_utils.h"
#include "utils/file_system_utils.h"

void launchGame(const std::string& version, const std::string& username) {
    // 获取 Java 路径
    std::string javaPath = "C:/Program Files/Java/jdk-17/bin/java.exe";
    
    // 构建游戏参数
    std::string gameDir = FileSystemUtils::getMinecraftDirectory();
    std::string versionDir = FileSystemUtils::getVersionDirectory(version);
    
    std::string args = "-Xmx4G -Xms2G ";
    args += "-Djava.library.path=" + versionDir + "/natives ";
    args += "-cp " + versionDir + "/minecraft.jar ";
    args += "net.minecraft.client.main.Main ";
    args += "--username " + username + " ";
    args += "--version " + version + " ";
    args += "--gameDir " + gameDir;
    
    // 启动游戏（非阻塞）
    bool success = ProcessUtils::launchMinecraft(javaPath, args);
    
    if (success) {
        std::cout << "Minecraft " << version << " 已启动！" << std::endl;
    } else {
        std::cerr << "启动失败！" << std::endl;
    }
}
```

### 示例2：启动游戏并监控状态

```cpp
void launchGameWithMonitoring(const std::string& version) {
    std::string javaPath = "java";
    std::string args = "-jar minecraft.jar";
    
    // 在后台启动，完成后执行回调
    ProcessUtils::launchInBackground(
        javaPath + " " + args,
        [version](int exitCode) {
            if (exitCode == 0) {
                std::cout << "[Monitor] Minecraft " << version 
                          << " 正常退出" << std::endl;
            } else {
                std::cerr << "[Monitor] Minecraft " << version 
                          << " 异常退出，退出码: " << exitCode << std::endl;
                
                // 这里可以显示错误信息或重启游戏
            }
        }
    );
    
    std::cout << "游戏已在后台启动，启动器继续运行" << std::endl;
    
    // 启动器可以做其他事情，比如显示游戏状态UI
}
```

### 示例3：打开 Minecraft 目录

```cpp
void openMinecraftFolder() {
    std::string mcDir = FileSystemUtils::getMinecraftDirectory();
    
    if (ProcessUtils::openFileOrFolder(mcDir)) {
        std::cout << "已打开 Minecraft 目录: " << mcDir << std::endl;
    } else {
        std::cerr << "无法打开目录" << std::endl;
    }
}
```

### 示例4：打开 Mod 文件夹

```cpp
void openModsFolder(const std::string& version) {
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    
    // 确保目录存在
    FileSystemUtils::ensureDirectoryExists(modsDir);
    
    // 打开文件夹
    ProcessUtils::openFileOrFolder(modsDir);
}
```

---

## 💡 高级用法

### 1. 并行启动多个进程

```cpp
// 同时启动多个后台任务
ProcessUtils::launchInBackground("task1", [](int code) {
    std::cout << "Task 1 completed: " << code << std::endl;
});

ProcessUtils::launchInBackground("task2", [](int code) {
    std::cout << "Task 2 completed: " << code << std::endl;
});

ProcessUtils::launchInBackground("task3", [](int code) {
    std::cout << "Task 3 completed: " << code << std::endl;
});

// 所有任务并行执行
```

### 2. 链式执行（一个完成后执行下一个）

```cpp
// 第一步：下载资源
ProcessUtils::launchInBackground("download_resources.sh", [](int code) {
    if (code == 0) {
        // 第二步：解压资源
        ProcessUtils::launchInBackground("extract_resources.sh", [](int code2) {
            if (code2 == 0) {
                // 第三步：启动游戏
                ProcessUtils::launchMinecraft("java", "-jar game.jar");
            }
        });
    }
});
```

### 3. 超时控制

```cpp
#include <future>
#include <chrono>

bool launchWithTimeout(const std::string& command, int timeoutSeconds) {
    auto future = std::async(std::launch::async, [command]() {
        return ProcessUtils::launchProcess(command, true);
    });
    
    // 等待指定时间
    auto status = future.wait_for(std::chrono::seconds(timeoutSeconds));
    
    if (status == std::future_status::ready) {
        return future.get();
    } else {
        std::cerr << "命令执行超时！" << std::endl;
        return false;
    }
}
```

---

## ⚠️ 注意事项

### 1. 线程安全

```cpp
// ❌ 错误：在回调中直接更新UI（可能不是主线程）
ProcessUtils::launchInBackground("command", [](int code) {
    updateUI(code);  // 可能导致崩溃！
});

// ✅ 正确：使用线程安全的通信机制
std::mutex mtx;
ProcessUtils::launchInBackground("command", [&mtx](int code) {
    std::lock_guard<std::mutex> lock(mtx);
    // 安全地更新共享数据
    sharedResult = code;
});
```

### 2. 资源管理

```cpp
// detach() 后的线程无法 join，确保不需要等待结果
ProcessUtils::launchInBackground("long_task", nullptr);

// 如果需要等待，使用 launchProcess 的同步模式
ProcessUtils::launchProcess("long_task", true);
```

### 3. 错误处理

```cpp
// 始终检查返回值
if (!ProcessUtils::launchProcess("command")) {
    std::cerr << "启动失败！" << std::endl;
    // 处理错误...
}
```

### 4. 路径中的空格

```cpp
// ❌ 错误：路径有空格时可能失败
ProcessUtils::launchProcess("C:/Program Files/Java/jdk-17/bin/java.exe");

// ✅ 正确：用引号包裹路径
ProcessUtils::launchProcess("\"C:/Program Files/Java/jdk-17/bin/java.exe\" -version");
```

---

## 📊 API 参考

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `launchProcess()` | command, waitForCompletion | bool | 启动进程，可选择是否等待 |
| `launchInBackground()` | command, callback | void | 后台执行，完成后调用回调 |
| `launchMinecraft()` | javaPath, gameArgs | bool | Minecraft专用启动方法 |
| `openFileOrFolder()` | path | bool | 打开文件或文件夹 |

---

## 🎯 下一步

现在 ProcessUtils 已经可以使用了，接下来可以：

1. ✅ 整合到 GameLauncher 模块
2. ✅ 实现完整的游戏启动流程
3. ✅ 添加进程监控功能
4. ⏳ 实现游戏日志实时显示

祝你开发顺利！🚀
