#include "launcher.h"
#include <iostream>
#include <string>
//#include <vector>
#include "minecraft/game_launcher.h"
#include "config/config_manager.h"
#include "minecraft/version_manager.h"
#include "java/java_detector.h"

Launcher::Launcher(bool init, std::string stts) {
    status = stts;
    initialized = false;
    if (init) {
        this->initialize();
    }
}
Launcher::Launcher(bool init) {
    *this = Launcher(init, "Launching");
}
Launcher::Launcher() {
    *this = Launcher(false, "NotReady");
}
Launcher::~Launcher() {
    // 释放 initialize() 创建的管理器实例，避免内存泄漏
    delete config;
    delete versionManager;
    config = nullptr;
    versionManager = nullptr;
    status = "NotReady";
    initialized = false;
}

bool Launcher::initialize() {
    std::cout << "[Launcher] Initializing..." << std::endl;
    
    // 1. 初始化配置管理器
    ConfigManager* config = new ConfigManager();
    if (!config->initialize("launcher_config.json")) {
        std::cerr << "[Launcher] Warning: Failed to initialize config, using defaults" << std::endl;
    }
    
    // 2. 初始化版本管理器
    VersionManager* versionManager = new VersionManager();
    
    // 3. 检测Java（如果未配置）
    if (config->getJavaPath().empty()) {
        std::cout << "[Launcher] Java path not configured, detecting..." << std::endl;
        JavaDetector javaDetector;
        
        // 根据当前版本推荐Java
        std::string currentVersion = config->getCurrentVersion();
        int mcVersion = 0;
        if (!currentVersion.empty()) {
            // 解析版本号，例如 "1.20.1" -> 120
            size_t dot1 = currentVersion.find('.');
            size_t dot2 = currentVersion.find('.', dot1 + 1);
            if (dot1 != std::string::npos && dot2 != std::string::npos) {
                std::string major = currentVersion.substr(0, dot1);
                std::string minor = currentVersion.substr(dot1 + 1, dot2 - dot1 - 1);
                mcVersion = std::stoi(major) * 100 + std::stoi(minor);
            }
        }
        
        if (mcVersion == 0) mcVersion = 120;  // 默认1.20
        
        std::string javaPath = javaDetector.getRecommendedJavaPath(mcVersion);
        if (!javaPath.empty()) {
            config->setJavaPath(javaPath);
            std::cout << "[Launcher] Auto-configured Java: " << javaPath << std::endl;
        } else {
            std::cerr << "[Launcher] Error: No suitable Java found!" << std::endl;
            delete config;
            delete versionManager;
            return false;
        }
    }
    
    // 4. 保存配置
    config->save();

    // 5. 存储管理器实例为成员变量（修复成功路径指针泄漏）
    /* 旧代码（bug）：成功路径既未存成员也未 delete，config/versionManager 指针泄漏
    // TODO: 将config和versionManager存储为Launcher的成员变量
    */
    this->config = config;
    this->versionManager = versionManager;

    initialized = true;
    status = "Ready";
    
    std::cout << "[Launcher] Initialization complete." << std::endl;
    return true;
}

bool Launcher::launchGame(const std::string& version, const std::string& accountName) {
    std::cout << "[Launcher] Launching game..." << std::endl;
    std::cout << "  Version: " << version << std::endl;
    std::cout << "  Account: " << accountName << std::endl;
    
    if (!initialized) {
        std::cerr << "[Launcher] Error: Launcher not initialized!" << std::endl;
        return false;
    }
    
    status = "Launching";

    /* 原有代码
    static GameLauncher gamelauncher=GameLauncher();
    gamelauncher.launchProcess("");
    */

    /* 旧代码（bug）：每次 new 临时 ConfigManager/VersionManager，既泄漏 initialize()
       已建的成员实例，又重复加载配置；正确做法是复用 initialize() 存储的成员
    // 创建临时配置和版本管理器（理想情况下应该从Launcher成员变量获取）
    ConfigManager* config = new ConfigManager();
    config->initialize("launcher_config.json");
    VersionManager* versionManager = new VersionManager();
    // 创建GameLauncher
    GameLauncher* gameLauncher = new GameLauncher(config, versionManager);
    // 清理
    delete gameLauncher;
    delete config;
    delete versionManager;
    */

    // 复用 initialize() 已创建并存储为成员的管理器实例
    GameLauncher* gameLauncher = new GameLauncher(this->config, this->versionManager);

    // 创建账户（临时使用离线账户）
    Account account;
    account.username = accountName;
    account.uuid = "";
    account.accessToken = "";
    account.type = AcctType::OffLine;
    account.isValid = true;

    // 启动游戏
    bool success = gameLauncher->launchGame(version, account);

    // 清理（GameLauncher 不持有 config/versionManager 所有权，仅删自身）
    delete gameLauncher;

    if (success) {
        status = "Running";
    } else {
        status = "Error";
    }

    return success;
}

std::string Launcher::getStatus() const {
    return status;
}
