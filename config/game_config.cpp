#include "game_config.h"
#include "../utils/json_parser.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif
namespace fs = std::filesystem;

GameConfig& GameConfig::instance() {
    static GameConfig inst;
    return inst;
}

GameConfig& GameCfg = GameConfig::instance();

// ===================== 占位符工具 =====================

std::string GameConfig::getUserName() {
#ifdef _WIN32
    const char* user = std::getenv("USERNAME");
#else
    const char* user = std::getenv("USER");
#endif
    if (user && *user) {
        return std::string(user);
    }
    return "Default";
}

std::string GameConfig::getAppDirPath() {
    // 启动器可执行文件所在目录
    try {
        fs::path exeDir = fs::current_path();
        return exeDir.string();
    } catch (...) {
#ifdef _WIN32
        return ".";
#else
        return ".";
#endif
    }
}

void replaceAll(std::string& s, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::string GameConfig::expandPath(const std::string& templ) {
    std::string s = templ;
    replaceAll(s, "${UserName}", getUserName());
    replaceAll(s, "${AppDirPath}", getAppDirPath());
    return s;
}

// ===================== 加载 / 保存 =====================

void GameConfig::ensureLoaded() {
    if (!m_loaded) {
        loadFromDisk();
    }
}

void GameConfig::loadFromDisk() {
    m_loaded = true;  // 先标记，避免递归
    json j;
    try {
        j = JsonParser::parseFile(game_config_file);
    } catch (...) {
        // 文件不存在或解析失败：写入默认配置
        std::string def = defaultConfigJson();
        j = JsonParser::parseString(def);
        JsonParser::saveToFile(j, game_config_file);
    }
    // 目前解析逻辑在 getMinecraftDirectory 中按需进行，这里不缓存
    // 如需性能优化，可在此处把 default/lists/current 解析进成员
}

void GameConfig::saveToDisk() const {
    // 当前配置在每次 getMinecraftDirectory 时从文件读取，
    // 若未来加入 setter，可在此统一持久化
}

std::string GameConfig::defaultConfigJson() const {
#ifdef _WIN32
    std::string slash = "\\\\";
    std::string everyonePath = "${AppDirPath}" + slash + ".minecraft";
    std::string userPath =
        "C:\\\\Users\\\\${UserName}\\\\AppData\\\\Roaming\\\\.minecraft";
#else
    std::string slash = "/";
    std::string everyonePath = "${AppDirPath}" + slash + ".minecraft";
    std::string userPath = "${HOME}/.minecraft";
#endif
    std::ostringstream oss;
    oss << "{\n"
        << "    \"directory\": {\n"
        << "        \"default\": {\n"
        << "            \"${UserName}\": \"" << userPath << "\",\n"
        << "            \"Everyone\": \"" << everyonePath << "\"\n"
        << "        },\n"
        << "        \"lists\": [],\n"
        << "        \"current\": \"Everyone\"\n"
        << "    }\n"
        << "}\n";
    return oss.str();
}

void GameConfig::reloadFromFile() {
    m_loaded = false;
    ensureLoaded();
}

// ===================== 目录获取 =====================

std::string GameConfig::getMinecraftDirectory() {
    ensureLoaded();

    json j;
    try {
        j = JsonParser::parseFile(game_config_file);
    } catch (...) {
        // 解析失败时 fallback 到启动器旁的 .minecraft
        return getAppDirPath() + "/.minecraft";
    }

    if (!j.contains("directory") || !j["directory"].is_object()) {
        return getAppDirPath() + "/.minecraft";
    }
    const json& dir = j["directory"];

    // 1) 确定 current
    std::string current = "Everyone";
    if (dir.contains("current") && dir["current"].is_string()) {
        current = dir["current"].get<std::string>();
    }

    // 2) 先在 lists 里按 name 查找
    if (dir.contains("lists") && dir["lists"].is_array()) {
        for (const auto& item : dir["lists"]) {
            if (item.contains("name") && item["name"].is_string() &&
                item["name"].get<std::string>() == current) {
                if (item.contains("path") && item["path"].is_string()) {
                    return expandPath(item["path"].get<std::string>());
                }
            }
        }
    }

    // 3) 再在 default 里查找（current 可能是 "${UserName}" 或 "Everyone"）
    if (dir.contains("default") && dir["default"].is_object()) {
        const json& def = dir["default"];
        // current 直接匹配
        if (def.contains(current) && def[current].is_string()) {
            return expandPath(def[current].get<std::string>());
        }
        // current == "${UserName}" 时，尝试用实际用户名匹配
        if (current == "${UserName}") {
            std::string uname = getUserName();
            if (def.contains(uname) && def[uname].is_string()) {
                return expandPath(def[uname].get<std::string>());
            }
        }
    }

    // 4) 兜底：Everyone
    if (dir.contains("default") && dir["default"].is_object() &&
        dir["default"].contains("Everyone") &&
        dir["default"]["Everyone"].is_string()) {
        return expandPath(dir["default"]["Everyone"].get<std::string>());
    }

    // 5) 最终兜底
    return getAppDirPath() + "/.minecraft";
}

std::string GameConfig::getAssetsDirectory() {
    return getMinecraftDirectory() + "/assets";
}

std::string GameConfig::getLibrariesDirectory() {
    return getMinecraftDirectory() + "/libraries";
}
