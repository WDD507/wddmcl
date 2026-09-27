#ifndef MOD_MANAGER_H
#define MOD_MANAGER_H

#include <string>
#include <vector>
#include "../utils/file_system_utils.h"

struct ModInfo {
    std::string name;            // Mod名称
    std::string version;         // Mod版本
    std::string file;            // 文件路径
    bool enabled;                // 是否启用
    std::string requiredLoader;  // 所需加载器 (forge/fabric)
};

class ModManager {
public:
    // 安装Mod
    bool installMod(
        const std::string& modFile,
        const std::string& version
    );

    // 卸载Mod
    bool uninstallMod(
        const std::string& modName,
        const std::string& version
    );

    // 列出已安装Mod
    std::vector<std::string> listMods(const std::string& version);

    // 启用/禁用Mod
    bool toggleMod(
        const std::string& modName,
        const std::string& version,
        bool enabled=false
    );

    // 检测Mod冲突
    std::vector<std::string> detectConflicts(const std::string& version);
};

#endif // MOD_MANAGER_H
