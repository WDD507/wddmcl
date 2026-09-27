#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <string>
#include "../app.h"

/**
 * 游戏配置管理器
 * 管理 game_config.json：启动器对游戏的配置（MC 目录等）
 *
 * JSON 结构：
 * {
 *   "directory": {
 *     "default": {
 *       "${UserName}": "C:\\Users\\${UserName}\\AppData\\Roaming\\.minecraft",
 *       "Everyone": "${AppDirPath}\\.minecraft"
 *     },
 *     "lists": [
 *       { "name": "MyMCDir", "path": "D:\\path\\to\\the\\folder", "settings": {} }
 *     ],
 *     "current": "Everyone"
 *   }
 * }
 *
 * 占位符（${...}）会被 expandPath 替换为实际值：
 *   ${UserName}    -> 当前系统用户名
 *   ${AppDirPath}  -> 启动器所在目录
 */
class GameConfig {
public:
    static GameConfig& instance();

    GameConfig(const GameConfig&)            = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    // ===== 目录相关 =====

    // 获取当前激活的 Minecraft 根目录（按 current 字段查找并展开占位符）
    std::string getMinecraftDirectory();

    // 获取 assets 目录（mcDir/assets）
    std::string getAssetsDirectory();

    // 获取 libraries 目录（mcDir/libraries）
    std::string getLibrariesDirectory();

    // ===== 占位符展开 =====

    // 将路径模板中的 ${UserName} / ${AppDirPath} 替换为实际值
    static std::string expandPath(const std::string& templ);

    // 获取当前系统用户名
    static std::string getUserName();

    // 获取启动器所在目录（不含末尾分隔符）
    static std::string getAppDirPath();

    // ===== 持久化 =====

    // 强制从磁盘重新加载
    void reloadFromFile();

private:
    GameConfig()  = default;
    ~GameConfig() = default;

    bool m_loaded = false;

    void ensureLoaded();
    void loadFromDisk();
    void saveToDisk() const;

    // 生成默认配置（首次运行时写入）
    std::string defaultConfigJson() const;
};

extern GameConfig& GameCfg;

#endif // GAME_CONFIG_H
