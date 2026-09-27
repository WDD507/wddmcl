#include "process_utils.h"
#include <string.h>

// ==================== 公共方法实现 ====================

bool ProcessUtils::launchProcess(const std::string& command, bool waitForCompletion) {
    if (waitForCompletion) {
        // 同步执行（阻塞）
        int exitCode = executeSync(command);
        return exitCode >= 0;
    } else {
        // 异步执行（非阻塞）
        return executeAsync(command);
    }
}

void ProcessUtils::launchInBackground(const std::string& command, 
                                      std::function<void(int)> callback) {
    // 使用 std::async 在后台线程中执行
    // 注意：我们不需要等待future，所以忽略返回值
    [[maybe_unused]] auto future = std::async(
        std::launch::async, [command, callback]() {
            int exitCode = executeSync(command);
            if (callback) {
                callback(exitCode);
            }
        }
    );
}

bool ProcessUtils::launchMinecraft(const std::string& javaPath,
                                   const std::string& gameArgs) {
    std::string fullCommand = javaPath + " " + gameArgs;
    
    std::cout << "[ProcessUtils] Launching Minecraft..." << std::endl;
    std::cout << "[ProcessUtils] Command: " << fullCommand << std::endl;
    
    // 在后台启动，不阻塞启动器
    return launchProcess(fullCommand, false);
}

bool ProcessUtils::openFileOrFolder(const std::string& path) {
#ifdef _WIN32
    // Windows: 使用 ShellExecute 打开文件或文件夹
    HINSTANCE result = ShellExecuteA(
        NULL,           // 父窗口
        "open",         // 操作
        path.c_str(),   // 路径
        NULL,           // 参数
        NULL,           // 工作目录
        SW_SHOWNORMAL   // 显示方式
    );
    
    return (intptr_t)result > 32;
    
#elif __APPLE__
    // macOS: 使用 open 命令
    std::string command = "open \"" + path + "\"";
    return executeAsync(command);
    
#else
    // Linux: 使用 xdg-open
    std::string command = "xdg-open \"" + path + "\"";
    return executeAsync(command);
#endif
}

// ==================== 私有方法实现 ====================

bool ProcessUtils::executeAsync(const std::string& command) {
#ifdef _WIN32
    // Windows: 使用 ShellExecute 异步启动
    HINSTANCE result = ShellExecuteA(
        NULL,
        "open",
        command.c_str(),
        NULL,
        NULL,
        SW_SHOWNORMAL
    );
    
    // 如果返回值大于32，表示成功
    return (intptr_t)result > 32;
    
#else
    // Linux/macOS: 使用 fork + exec
    pid_t pid = fork();
    
    if (pid < 0) {
        // fork 失败
        std::cerr << "[ProcessUtils] fork 失败" << std::endl;
        return false;
    }
    
    if (pid == 0) {
        // 子进程：执行命令
        execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
        // 如果 execl 返回，说明执行失败
        _exit(127);
    }
    
    // 父进程：立即返回，不等待子进程
    return true;
#endif
}

int ProcessUtils::executeSync(const std::string& command) {
#ifdef _WIN32
    // Windows: 使用 CreateProcess 同步执行
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // 创建命令行副本（CreateProcess会修改它）
    char* cmd = new char[command.length() + 1];
    strcpy(cmd, command.c_str());
    
    bool success = CreateProcessA(
        NULL,           // 应用程序名
        cmd,            // 命令行
        NULL,           // 进程安全属性
        NULL,           // 线程安全属性
        FALSE,          // 不继承句柄
        0,              // 创建标志
        NULL,           // 环境变量
        NULL,           // 当前目录
        &si,            // 启动信息
        &pi             // 进程信息
    );
    
    delete[] cmd;
    
    if (!success) {
        std::cerr << "[ProcessUtils] CreateProcess 失败，错误码: " 
                  << GetLastError() << std::endl;
        return -1;
    }
    
    // 等待进程完成
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    // 获取退出码
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    // 清理
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return static_cast<int>(exitCode);

#else
    // Linux/macOS: 使用 system() 同步执行
    return system(command.c_str());
#endif
}

// ==================== PID 回传扩展实现 ====================

bool ProcessUtils::launchProcess(const std::string& command, bool waitForCompletion, std::string& outPid) {
    if (waitForCompletion) {
        // 同步执行：executeSync 不回传 PID，清空 outPid 以示语义
        outPid.clear();
        int exitCode = executeSync(command);
        return exitCode >= 0;
    } else {
        // 异步执行并回传 PID
        return executeAsync(command, outPid);
    }
}

bool ProcessUtils::executeAsync(const std::string& command, std::string& outPid) {
#ifdef _WIN32
    // Windows: 使用 CreateProcess 异步启动并获取子进程 PID
    // （ShellExecuteA 不返回 PID，故此处改用 CreateProcessA）
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA 会修改命令行缓冲区，需拷贝一份可写副本
    char* cmd = new char[command.length() + 1];
    strcpy(cmd, command.c_str());

    bool success = CreateProcessA(
        NULL,           // 应用程序名（NULL 表示从命令行解析）
        cmd,            // 命令行（可写副本）
        NULL,           // 进程安全属性
        NULL,           // 线程安全属性
        FALSE,          // 不继承句柄
        0,              // 创建标志
        NULL,           // 环境变量
        NULL,           // 当前目录
        &si,            // 启动信息
        &pi             // 进程信息
    );
    delete[] cmd;

    if (!success) {
        std::cerr << "[ProcessUtils] CreateProcess 失败，错误码: "
                  << GetLastError() << std::endl;
        return false;
    }

    outPid = std::to_string(pi.dwProcessId);

    // 关闭本进程持有的句柄副本（不影响子进程继续运行，实现非阻塞）
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
#else
    // Linux/macOS: fork + execl，父进程直接获得子进程 PID
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "[ProcessUtils] fork 失败" << std::endl;
        return false;
    }
    if (pid == 0) {
        // 子进程：执行命令
        execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    // 父进程：回传 PID 后立即返回
    outPid = std::to_string(pid);
    return true;
#endif
}
