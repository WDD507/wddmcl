#ifndef GAME_LAUNCHER_H
#define GAME_LAUNCHER_H

#include <cstring>
#include <string>
#include <vector>
#include "../app.h"
#include "../utils/file_system_utils.h"
#include "../utils/process_utils.h"
#include "../account/account_manager.h"
#include "../config/config_manager.h"
#include "version_manager.h"
#ifdef _WIN32
#ifndef PROCESS_SYNCHRONIZE
#define PROCESS_SYNCHRONIZE  0x00100000
#endif
#include <winnt.h>
#include <winbase.h>
#include <Windows.h>
#include <TlHelp32.h>
#elif defined(__linux__)
//
#elif defined(WDDMCL_PASS)
//
#else
//
#endif
#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

class GameLauncher {
private:
    ConfigManager* config;
    VersionManager* versionManager;
    std::string currentProcessId;  // 用于跟踪游戏进程
    
public:
    GameLauncher(ConfigManager* cfg, VersionManager* vm);
    ~GameLauncher();

    // 启动游戏（高级接口）
    bool launchGame(
        const std::string& version,
        const Account& account
    );
    
    // 构建启动命令
    std::string buildLaunchCommand(
        const std::string& javaPath,
        const std::string& version,
        const Account& account
    );
    
    // 获取类路径（classpath）
    std::string buildClasspath(const std::string& version);
    std::string buildLibrarypath(const std::string& version);
    
    // 获取JVM参数
    std::string buildJvmArgs(const std::string& version);
    
    // 获取游戏参数
    std::string buildGameArgs(
        const std::string& version,
        const Account& account
    );
    
    // 启动游戏进程
    bool launchProcess(const std::string& command);
    
    // 等待游戏结束
    int waitForGameExit(unsigned int dwTimeout=INFINITE);
    
    // 终止游戏
    bool terminateGame();
};

#endif // GAME_LAUNCHER_H
