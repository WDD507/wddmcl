#ifndef FORGE_INSTALLER_H
#define FORGE_INSTALLER_H

#include <string>
#include <vector>

class ForgeInstaller {
public:
    // 获取可用Forge版本
    std::vector<std::string> getAvailableForgeVersions(const std::string& mcVersion);
    
    // 下载Forge安装器
    bool downloadForgeInstaller(const std::string& forgeVersion);
    
    // 安装Forge
    bool installForge(const std::string& forgeVersion, const std::string& mcVersion);
    
    // 检查Forge是否已安装
    bool isForgeInstalled(const std::string& version);
};

#endif // FORGE_INSTALLER_H
