#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <string>
#include <vector>

class ConfigManager;
class VersionManager;

class Launcher {
public:
    Launcher(bool init, std::string stts);
    Launcher(bool init);
    Launcher();
    ~Launcher();

    // 初始化启动器
    bool initialize();
    
    // 启动游戏（主入口）
    bool launchGame(const std::string& version, const std::string& account);
    
    // 获取启动器状态
    std::string getStatus() const;
    
private:
    std::string status;
    bool initialized;
    ConfigManager* config = nullptr;           // 配置管理器（initialize 创建，析构释放）
    VersionManager* versionManager = nullptr;  // 版本管理器（同上）
};

#endif // LAUNCHER_H
