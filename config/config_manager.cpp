#include "config_manager.h"
#include <iostream>
#include <string>

ConfigManager CfgMgr=ConfigManager(launcher_config_file);

ConfigManager::ConfigManager(const std::string &configPath) {
    resetToDefault();
    initialize(configPath);
}
ConfigManager::ConfigManager() {
    resetToDefault();
}
ConfigManager::~ConfigManager() {
    save();  // 析构时自动保存
}

bool ConfigManager::initialize(const std::string& configPath) {
    std::cout << "[ConfigManager] Initializing..." << std::endl;
    
    configFile = configPath;
    
    // 尝试加载现有配置
    if (FileSystemUtils::fileExists(configFile)) {
        try {
            configData = JsonParser::parseFile(configFile);
            std::cout << "[ConfigManager] Configuration loaded successfully" << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[ConfigManager] Failed to load config: " << e.what() << std::endl;
            std::cout << "[ConfigManager] Using default configuration" << std::endl;
            resetToDefault();
            return false;
        }
    } else {
        std::cout << "[ConfigManager] No config file found, creating default" << std::endl;
        resetToDefault();
        save();
        return true;
    }
}

bool ConfigManager::save() {
    try {
        bool success = JsonParser::saveToFile(configData, configFile);
        if (success) {
            std::cout << "[ConfigManager] Configuration saved" << std::endl;
        }
        return success;
    } catch (const std::exception& e) {
        std::cerr << "[ConfigManager] Failed to save config: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    if (configData.contains(key)) {
        return configData[key].get<std::string>();
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    if (configData.contains(key)) {
        return configData[key].get<int>();
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    if (configData.contains(key)) {
        return configData[key].get<bool>();
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    if (configData.contains(key)) {
        return configData[key].get<double>();
    }
    return defaultValue;
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    configData[key] = value;
}

void ConfigManager::setInt(const std::string& key, int value) {
    configData[key] = value;
}

void ConfigManager::setBool(const std::string& key, bool value) {
    configData[key] = value;
}

void ConfigManager::setDouble(const std::string& key, double value) {
    configData[key] = value;
}

bool ConfigManager::hasKey(const std::string& key) const {
    return configData.contains(key);
}

void ConfigManager::removeKey(const std::string& key) {
    if (configData.contains(key)) {
        configData.erase(key);
    }
}

void ConfigManager::resetToDefault() {
    std::cout << "[ConfigManager] Resetting to default configuration..." << std::endl;
    
    configData.clear();
    configData["version"] = "1.0";
    configData["java_path"] = "";
    configData["memory_mb"] = 4096;
    configData["fullscreen"] = false;
    configData["current_version"] = "";
    configData["current_account"] = "";
    configData["render_distance"] = 12;
    configData["auto_update"] = true;
    configData["language"] = "zh_CN";
}

json ConfigManager::getAllConfig() const {
    return configData;
}

// ==================== Minecraft特定配置 ====================

std::string ConfigManager::getJavaPath() const {
    return getString("java_path", "");
}

void ConfigManager::setJavaPath(const std::string& path) {
    setString("java_path", path);
}

int ConfigManager::getMemoryMB() const {
    return getInt("memory_mb", 4096);
}

void ConfigManager::setMemoryMB(int mb) {
    setInt("memory_mb", mb);
}

std::string ConfigManager::getCurrentVersion() const {
    return getString("current_version", "");
}

void ConfigManager::setCurrentVersion(const std::string& version) {
    setString("current_version", version);
}

std::string ConfigManager::getCurrentAccount() const {
    return getString("current_account", "");
}

void ConfigManager::setCurrentAccount(const std::string& username) {
    setString("current_account", username);
}

bool ConfigManager::getFullscreen() const {
    return getBool("fullscreen", false);
}

void ConfigManager::setFullscreen(bool fullscreen) {
    setBool("fullscreen", fullscreen);
}

int ConfigManager::getRenderDistance() const {
    return getInt("render_distance", 12);
}

void ConfigManager::setRenderDistance(int distance) {
    setInt("render_distance", distance);
}
