#include "game_launcher.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#ifdef _WIN32
#include <handleapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <winnt.h>
#elif 0
#else
#endif

namespace {
    VersionManager &vermgr=VersionManager::instance();
}

GameLauncher::GameLauncher(ConfigManager* cfg, VersionManager* vm) 
    : config(cfg), versionManager(vm), currentProcessId("") {
    std::cout << "[GameLauncher] GameLauncher initialized." << std::endl;
}

GameLauncher::~GameLauncher() {
    // 确保游戏进程已终止
    if (!currentProcessId.empty()) {
        terminateGame();
    }
    std::cout << "[GameLauncher] GameLauncher destroyed." << std::endl;
}

bool GameLauncher::launchGame(const std::string& version, const Account& account) {
    std::cout << "[GameLauncher] Launching game: " << version << std::endl;
    
    // 1. 检查版本是否安装
    if (!versionManager->isVersionInstalled(version)) {
        std::cerr << "[GameLauncher] Error: Version " << version << " is not installed!" << std::endl;
        return false;
    }
    
    // 2. 获取Java路径
    std::string javaPath = config->getJavaPath();
    if (javaPath.empty()) {
        std::cerr << "[GameLauncher] Error: Java path not configured!" << std::endl;
        return false;
    }
    
    // 3. 构建启动命令
    std::string command = buildLaunchCommand(javaPath, version, account);
    
    if (command.empty()) {
        std::cerr << "[GameLauncher] Error: Failed to build launch command!" << std::endl;
        return false;
    }
    
    std::cout << "[GameLauncher] Launch command: " << command << std::endl;
    
    // 4. 启动游戏
    bool success = launchProcess(command);
    
    if (success) {
        std::cout << "[GameLauncher] Game launched successfully!" << std::endl;
    } else {
        std::cerr << "[GameLauncher] Error: Failed to launch game!" << std::endl;
    }
    
    return success;
}

std::string GameLauncher::buildLaunchCommand(
    const std::string& javaPath,
    const std::string& version,
    const Account& account
) {
    std::cout << "[GameLauncher] Building launch command..." << std::endl;
    
    /* 原有代码 - 已改进
    std::string command = javaPath + " -jar ";
    command += std::string(
        FileSystemUtils::getVersionDirectory(version) +
#ifdef _WIN32
        "\\"
#else
        "/"
#endif
        + version + ".jar"
    );
    command += std::string(" --username " + account.username);
    command += std::string(" --accessToken " + account.accessToken);
    command += std::string(" --uuid " + account.uuid);
    */
    
    std::ostringstream cmd;
    
    // Java路径
    cmd << "\"" << javaPath << "\"";
    
    // JVM参数
    cmd << " " << buildJvmArgs(version);
    
    // 类路径
    cmd << " -cp \"" << buildClasspath(version) << "\"";
    
    // 主类 - 从version.json读取mainClass字段
    // 原版: net.minecraft.client.main.Main
    // Forge: cpw.mods.modlauncher.Launcher
    // Fabric: net.fabricmc.loader.impl.launch.knot.KnotClient
    // NeoForge: cpw.mods.modlauncher.Launcher
    // Quilt: org.quiltmc.loader.impl.launch.knot.KnotClient
    std::string mainClass = "net.minecraft.client.main.Main"; // 默认原版主类

    // 从version.json读取mainClass
    std::string versionJsonPath = FileSystemUtils::getVersionDirectory(version) + "/" + version + ".json";
    if (FileSystemUtils::fileExists(versionJsonPath)) {
        try {
            json versionJson = JsonParser::parseFile(versionJsonPath);
            if (versionJson.contains("mainClass") && !versionJson["mainClass"].is_null()) {
                mainClass = versionJson["mainClass"].get<std::string>();
                std::cout << "[GameLauncher] Found mainClass in version.json: " << mainClass << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[GameLauncher] Failed to read mainClass from version.json: " << e.what() << std::endl;
        }
    }
    /*
    // 主类
    if (0) { // 带模组加载器（不同的加载器有不同的入口点）
        cmd << ""; // TODO
    }else{ // 原版
        cmd << " net.minecraft.client.main.Main";
    }
    */

    cmd << " " << mainClass;
    
    // 游戏参数
    cmd << " " << buildGameArgs(version, account);
    
    return cmd.str();
}

std::string GameLauncher::buildClasspath(const std::string& version) {
    std::cout << "[GameLauncher] Building classpath for version: " << version << std::endl;
    
    std::string versionDir = FileSystemUtils::getVersionDirectory(version);
    //std::string librariesDir = FileSystemUtils::getLibrariesDirectory();
    
    std::ostringstream cp;
    
#ifdef _WIN32
    // Windows使用分号分隔
    cp << versionDir << "/" << version << ".jar";
    cp << ";";
    
    // DONE(2026/9/13 ??:??): 添加库文件的classpath
    // 需要解析version.json中的libraries列表
    //cp << librariesDir << "/*";
    cp << buildLibrarypath(version);
#else
    // Linux/macOS使用冒号分隔
    cp << versionDir << "/" << version << ".jar";
    cp << ":";
    //cp << librariesDir << "/*";
    cp << buildLibrarypath(version);
#endif
    return cp.str();
}
std::string GameLauncher::buildLibrarypath(const std::string& version) {
    std::string versionDir = FileSystemUtils::getVersionDirectory(version);
    std::string librariesDir = FileSystemUtils::getLibrariesDirectory();
    std::ostringstream cp;
    json j=JsonParser::parseFile(
        versionDir + FileSystemUtils::getSlash() + version + ".json"
    );
    for (const json &libj : j["libraries"].get<std::vector<json> >()) {
        if (vermgr.shouldIncludeLibrary(
            libj,
            App::getCurrentOSName(),
            App::getCurrentOSVersion()
        )){
            if(libj["downloads"].contains("artifact")) {
                cp << librariesDir << "/"//FileSystemUtils::getSlash()
                    << libj["downloads"]["artifact"]["path"].get<std::string>()
                    << " ";
            }
            /*if(
                libj["downloads"].contains("classifiers")
            ){
                std::string ntv="";
                if(
                    (libj.contains("natives")) &&
                    (libj["natives"].contains(App::getCurrentOSName()))
                ){
                    ntv = libj["natives"][App::getCurrentOSName()]
                        .get<std::string>();
                }else{
                    ntv = "natives-" + App::getCurrentOSName();
                }
                if (libj.contains(ntv)) {
                    cp << librariesDir << "/"
                        << libj["downloads"]["classifiers"][ntv]["path"]
                            .get<std::string>()
                        << " ";/
                }
            }*/
        }
    }
    return cp.str();
}

std::string GameLauncher::buildJvmArgs(const std::string& version) {
    std::cout << "[GameLauncher] Building JVM arguments..." << std::endl;
    
    std::ostringstream args;
    
    // 内存设置
    int memoryMB = config->getMemoryMB();
    args << "-Xmx" << memoryMB << "M";
    args << " -Xms" << (memoryMB / 2) << "M";
    
    // 系统属性
    args << " -Djava.library.path=" << FileSystemUtils::getVersionDirectory(version)
        << "/natives";
    args << " -Dminecraft.applet.target_directory="
        << FileSystemUtils::getMinecraftDirectory();
    // 启动器信息
    args << " -Dminecraft.launcher.brand=" << launcher_id_name;
    args << " -Dminecraft.launcher.version=" << launcher_ver;
    
    // 其他JVM参数
    args << " -XX:+UseConcMarkSweepGC";
    args << " -XX:+CMSIncrementalMode";
    args << " -XX:-UseAdaptiveSizePolicy";
    args << " -Xmn128M";
    
    return args.str();
}

std::string GameLauncher::buildGameArgs(
    const std::string& version,
    const Account& account
) {
    std::cout << "[GameLauncher] Building game arguments..." << std::endl;
    
    std::ostringstream args;
    
    // 版本信息
    args << "--version " << version;
    
    // 游戏目录
    args << " --gameDir " << FileSystemUtils::getMinecraftDirectory();
    
    // 资源目录
    args << " --assetsDir " << FileSystemUtils::getAssetsDirectory();
    
    // 资产索引（根据版本确定）
    args << " --assetIndex " << version;
    
    // 账户信息
    args << " --username " << account.username;
    args << " --accessToken " << (account.accessToken.empty() ? "0" : account.accessToken);
    args << " --uuid " << 
        (account.uuid.empty() ? "00000000-0000-0000-0000-000000000000" : account.uuid);
    
    // 用户类型
    args << " --userType " << (account.type == AcctType::OffLine ? "legacy" : "mojang");
    
    // 窗口设置
    if (!config->getFullscreen()) {
        args << " --width 854 --height 480";
    } else {
        args << " --fullscreen true";
    }
    
    return args.str();
}

bool GameLauncher::launchProcess(const std::string& command) {
    std::cout << "[GameLauncher] Launching process..." << std::endl;

    /* 原有代码
    system(command.c_str());
    */

    /* 旧代码（bug）：两参 launchProcess 不回传 PID，导致 currentProcessId 始终为空，
       waitForGameExit/terminateGame 因 std::stoul("") 抛异常而失效
    bool success = ProcessUtils::launchProcess(command, false);
    */

    // 使用ProcessUtils启动游戏（非阻塞）并回传 PID 以便后续等待/终止
    std::string pid;
    bool success = ProcessUtils::launchProcess(command, false, pid);
    if (success && !pid.empty()) {
        this->currentProcessId = pid;
        std::cout << "[GameLauncher] Game process started in background, PID=" << pid << std::endl;
    }

    return success;
}

int GameLauncher::waitForGameExit(unsigned int dwTimeout) {
    std::cout << "[GameLauncher] Waiting for game exit..." << std::endl;
    
    // DONE(2026/6/10 20:20): 实现等待游戏进程结束
    // 需要使用ProcessUtils的同步模式或监控进程ID
#ifdef _WIN32
    // 打开进程：仅需要「同步权限」(用于等待)，无需读写/终止权限
    DWORD dwPid=static_cast<DWORD>(std::stoul(this->currentProcessId));
    HANDLE hProcess = OpenProcess(
        PROCESS_SYNCHRONIZE,
        FALSE, dwPid
    );
    if (hProcess == NULL)
    {
        // 错误码: ERROR_INVALID_PARAMETER=PID不存在; ERROR_ACCESS_DENIED=权限不足
        return FALSE;
    }

    // 阻塞等待进程句柄变为有信号(进程退出)
    DWORD dwWaitRet = WaitForSingleObject(
        hProcess, static_cast<DWORD>(dwTimeout));

    // 释放内核句柄（必须，防止资源泄漏）
    CloseHandle(hProcess);

    return static_cast<int>(dwWaitRet);
#elif defined(__linux__) || defined(WDDMCL_PASS)
    int status = 0;
    // 阻塞等待指定子进程，WUNTRACED：捕获子进程暂停/退出
    pid_t pid = static_cast<pid_t>(std::stoi(this->currentProcessId));
    waitpid(pid, &status, WUNTRACED);
    return status;
#elif defined(WDDMCL_PASS)
    //
#else
    std::cout << "[GameLauncher] WARNING: waitForGameExit not fully implemented"
        << std::endl;
#endif
    
    std::cout << "[GameLauncher] WARNING: waitForGameExit not fully implemented"
        << std::endl;
    return 0;
}

bool GameLauncher::terminateGame() {
    std::cout << "[GameLauncher] Terminating game..." << std::endl;
    
    // DONE(2026/6/9 13:10): 实现终止游戏进程
    // Windows: TerminateProcess
    // Linux/macOS: kill
    try {
#ifdef _WIN32
        DWORD pid=static_cast<DWORD>(std::stoul(this->currentProcessId));
        HANDLE hProcess=OpenProcess(
            PROCESS_TERMINATE, FALSE, pid
        );
        if (hProcess == NULL) {
            //
            return false;
        }
        BOOL success=TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return success != 0;
#elif defined(__linux__)
        pid_t pid=static_cast<pid_t>(std::stoi(this->currentProcessId));
        return kill(pid, SIGKILL);
#elif defined(WDDMCL_PASS)
        throw std::exception("抱歉，鸿蒙用户，敬请期待。");
#else
        throw std::exception("敬请期待");
#endif
    } catch (const std::invalid_argument &e) {
        std::cerr << "[GameLauncher] PID invaild arguments: " << e.what() << std::endl;
        return false;
    } catch (const std::out_of_range &e) {
        std::cerr << "[GameLauncher] PID out of range: " << e.what() << std::endl;
        return false;
    } catch (const std::exception &e) {
        std::cerr << "[GameLauncher] PID Error: " << e.what() << std::endl;
        return false;
    }
    
    //std::cout <<
    //    "[GameLauncher] WARNING: terminateGame not fully implemented" <<
    //    std::endl;
    return false;
}
