#include "mod_manager.h"
#include <iostream>
#include <string>
#include <algorithm>

bool ModManager::installMod(
    const std::string& modFile,
    const std::string& version
) {
    std::cout << "[ModManager] Installing mod: " << modFile <<
        " for version " << version << std::endl;
    
    // 检查mod文件是否存在
    if (!FileSystemUtils::fileExists(modFile)) {
        std::cerr << "[ModManager] Mod file not found: " << modFile << std::endl;
        return false;
    }
    
    // 获取mods目录
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    
    // 确保目录存在
    if (!FileSystemUtils::ensureDirectoryExists(modsDir)) {
        std::cerr << "[ModManager] Failed to create mods directory" << std::endl;
        return false;
    }
    
    // 提取文件名
    std::string fileName = FileSystemUtils::getFileName(modFile);
    std::string destPath = modsDir + "/" + fileName;
    
    // 复制文件
    if (FileSystemUtils::copyFile(modFile, destPath)) {
        std::cout << "[ModManager] Mod installed successfully: " << fileName << std::endl;
        return true;
    } else {
        std::cerr << "[ModManager] Failed to copy mod file" << std::endl;
        return false;
    }
}

bool ModManager::uninstallMod(
    const std::string& modName,
    const std::string& version
) {
    std::cout << "[ModManager] Uninstalling mod: " << modName <<
        " from version " << version << std::endl;
    
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    std::string modPath = modsDir + "/" + modName;
    
    // 尝试删除.jar文件
    if (modPath.length() < 4 || modPath.substr(modPath.length() - 4) != ".jar") {
        modPath += ".jar";
    }
    
    if (FileSystemUtils::fileExists(modPath)) {
        if (FileSystemUtils::deleteFile(modPath)) {
            std::cout << "[ModManager] Mod uninstalled successfully" << std::endl;
            return true;
        }
    }
    
    // 尝试删除.disabled文件
    std::string disabledPath = modPath + ".disabled";
    if (FileSystemUtils::fileExists(disabledPath)) {
        if (FileSystemUtils::deleteFile(disabledPath)) {
            std::cout << "[ModManager] Disabled mod removed" << std::endl;
            return true;
        }
    }
    
    std::cerr << "[ModManager] Mod not found: " << modName << std::endl;
    return false;
}

std::vector<std::string> ModManager::listMods(const std::string& version) {
    std::cout << "[ModManager] Listing mods for version: " << version << std::endl;
    
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    
    // 列出所有.jar文件（包括.disabled）
    std::vector<std::string> allFiles = FileSystemUtils::listFiles(modsDir, ".jar");
    
    // 过滤掉.disabled后缀的文件，只返回启用的mod
    std::vector<std::string> enabledMods;
    for (const auto& file : allFiles) {
        if (file.length() < 9 || file.substr(file.length() - 9) != ".disabled") {
            enabledMods.push_back(file);
        }
    }
    
    std::cout << "[ModManager] Found " << enabledMods.size() << " enabled mods" << std::endl;
    return enabledMods;
}

bool ModManager::toggleMod(
    const std::string& modName,
    const std::string& version,
    bool enabled
) {
    std::cout << "[ModManager] " << (enabled ? "Enabling" : "Disabling") <<
        " mod: " << modName << std::endl;
    
    std::string modsDir = FileSystemUtils::getModsDirectory(version);
    std::string modPath = modsDir + "/" + modName;
    
    // 确保有.jar后缀
    if (modPath.length() < 4 || modPath.substr(modPath.length() - 4) != ".jar") {
        modPath += ".jar";
    }
    
    if (enabled) {
        // 启用：移除.disabled后缀
        std::string disabledPath = modPath + ".disabled";
        
        if (FileSystemUtils::fileExists(disabledPath)) {
            if (FileSystemUtils::moveFile(disabledPath, modPath)) {
                std::cout << "[ModManager] Mod enabled: " << modName << std::endl;
                return true;
            }
        } else {
            std::cout << "[ModManager] Mod is already enabled" << std::endl;
            return true;
        }
    } else {
        // 禁用：添加.disabled后缀
        std::string disabledPath = modPath + ".disabled";
        
        if (FileSystemUtils::fileExists(modPath)) {
            if (FileSystemUtils::moveFile(modPath, disabledPath)) {
                std::cout << "[ModManager] Mod disabled: " << modName << std::endl;
                return true;
            }
        } else {
            std::cerr << "[ModManager] Mod not found: " << modName << std::endl;
            return false;
        }
    }
    
    return false;
}

std::vector<std::string> ModManager::detectConflicts(
    const std::string& version
) {
    std::cout << "[ModManager] Detecting conflicts for version: " << version << std::endl;
    
    // TODO: 实现真正的冲突检测
    // 1. 读取每个mod的mcmod.info或fabric.mod.json
    // 2. 检查依赖关系
    // 3. 检查版本兼容性
    // 4. 检查已知的冲突对
    
    std::vector<std::string> conflicts;
    
    // 示例：简单的同名mod检测
    auto mods = listMods(version);
    std::vector<std::string> modNames;
    
    for (const auto& mod : mods) {
        std::string name = FileSystemUtils::getFileNameWithoutExtension(mod);
        
        // 检查是否已有同名mod
        if (std::find(modNames.begin(), modNames.end(), name) != modNames.end()) {
            conflicts.push_back("Duplicate mod detected: " + name);
        } else {
            modNames.push_back(name);
        }
    }
    
    if (conflicts.empty()) {
        std::cout << "[ModManager] No conflicts detected" << std::endl;
    } else {
        std::cout << "[ModManager] Found " << conflicts.size()
            << " potential conflicts" << std::endl;
    }
    
    return conflicts;
}
