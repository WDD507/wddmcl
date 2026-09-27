#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <cstddef>
#include <exception>
#include <string>
#include <vector>
#include "wddwxy_json.h"
#include "../third_party/nlohmann/json.hpp"

using json = nlohmann::json;

struct Library {
    std::string name;
    std::string version;
    std::string path;

    Library(const std::string &_name,
        const std::string &_ver, const std::string &_path);
    Library();
    ~Library() = default;
};

namespace JsonParser {
    // 解析JSON文件
    json parseFile(const std::string& filePath);
    
    // 解析JSON字符串
    json parseString(const std::string& jsonString);
    
    // 保存JSON到文件
    bool saveToFile(const json& data, const std::string& filePath);
    
    // 从版本JSON提取信息
    // VersionInfo extractVersionInfo(const json& versionJson);
    
    // 从库JSON提取依赖
    std::vector<Library> extractLibraries(const json& versionJson);
}

#endif // JSON_PARSER_H
