#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include <string>
#include <thread>
#include <functional>
#include <iostream>
#include <future>

#ifdef _WIN32
    // Windows
    #include <windows.h>
    #include <shellapi.h>
    #ifdef _MSC_VER
    #pragma comment(lib, "shell32.lib")
    #endif
#else
    // Linux/macOS/Unix
    #include <cstdlib>
    #include <sys/wait.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <signal.h>
#endif

/**
 * 进程工具类
 * 提供非阻塞的外部进程启动功能（跨平台）
 */
class ProcessUtils {
public:
    /**
     * 在后台启动外部进程（非阻塞）
     * @param command 要执行的命令或要打开的文件路径
     * @param waitForCompletion 是否等待进程完成（默认false，不等待）
     * @return 是否成功启动
     */
    static bool launchProcess(const std::string& command, bool waitForCompletion = false);

    /**
     * 在后台启动外部进程并回传进程ID（非阻塞）
     * @param command 要执行的命令
     * @param waitForCompletion 是否等待完成（true 时 outPid 不回传）
     * @param outPid [out] 成功时写入进程ID字符串，失败时保持不变
     * @return 是否成功启动
     */
    static bool launchProcess(const std::string& command, bool waitForCompletion, std::string& outPid);
    
    /**
     * 在后台线程中执行命令（完全非阻塞）
     * @param command 命令
     * @param callback 完成后的回调函数（可选）
     */
    static void launchInBackground(const std::string& command, 
                                   std::function<void(int)> callback = nullptr);
    
    /**
     * Minecraft启动器专用：启动游戏进程
     * @param javaPath Java可执行文件路径
     * @param gameArgs 游戏启动参数
     * @return 是否成功启动
     */
    static bool launchMinecraft(const std::string& javaPath,
                                const std::string& gameArgs);
    
    /**
     * 打开文件或目录（跨平台）
     * @param path 文件或目录路径
     * @return 是否成功
     */
    static bool openFileOrFolder(const std::string& path);
    
private:
    /**
     * 异步执行（立即返回，不等待）
     */
    static bool executeAsync(const std::string& command);

    /**
     * 异步执行并回传进程ID（立即返回，不等待）
     * @param command 命令
     * @param outPid [out] 成功时写入进程ID字符串
     * @return 是否成功启动
     */
    static bool executeAsync(const std::string& command, std::string& outPid);
    
    /**
     * 同步执行（等待完成）
     */
    static int executeSync(const std::string& command);
};

#endif // PROCESS_UTILS_H
