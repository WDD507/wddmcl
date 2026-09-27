#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>
#include "../utils/json_parser.h"
#include "../utils/file_system_utils.h"
#include "../app.h"

using json = nlohmann::json;

/**
 * 配置管理器
 * 使用JSON格式存储和管理启动器配置
 */
class ConfigManager {
private:
    std::string configFile = launcher_config_file;
    json configData;
    
public:
    ConfigManager(const std::string &configPath);
    ConfigManager();
    ~ConfigManager();
    
    // 初始化（加载配置文件）
    bool initialize(const std::string& configPath = launcher_config_file);
    
    // 保存配置到文件
    bool save();
    
    // 获取配置项
    std::string getString(
        const std::string& key, const std::string& defaultValue = ""
    ) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    // 设置配置项
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setBool(const std::string& key, bool value);
    void setDouble(const std::string& key, double value);
    
    // 检查配置项是否存在
    bool hasKey(const std::string& key) const;
    
    // 删除配置项
    void removeKey(const std::string& key);
    
    // 重置为默认配置
    void resetToDefault();
    
    // 获取所有配置（用于调试）
    json getAllConfig() const;
    
    // Minecraft特定配置
    std::string getJavaPath() const;
    void setJavaPath(const std::string& path);
    
    int getMemoryMB() const;
    void setMemoryMB(int mb);
    
    std::string getCurrentVersion() const;
    void setCurrentVersion(const std::string& version);
    
    std::string getCurrentAccount() const;
    void setCurrentAccount(const std::string& username);
    
    bool getFullscreen() const;
    void setFullscreen(bool fullscreen);
    
    int getRenderDistance() const;
    void setRenderDistance(int distance);
};
extern ConfigManager CfgMgr;

#endif // CONFIG_MANAGER_H
