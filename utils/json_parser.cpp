#include <iostream>
#include <fstream>
#include <string>
#include "json_parser.h"

Library::Library(const std::string &_name,
    const std::string &_ver, const std::string &_path)  {
    this->name = _name;
    this->version = _ver;
    this->path = _path;
}
Library::Library() {
    *this = Library("", "", "");
}
/*Library::~Library() {
    this->name = this->version = this->path = "";
}*/

namespace JsonParser {
    json parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }
        
        json j;
        file >> j;
        return j;
    }
    
    json parseString(const std::string& jsonString) {
        return json::parse(jsonString);
    }
    
    bool saveToFile(const json& data, const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << data.dump(4);  // 4空格缩进
        return true;
    }
    
    std::vector<Library> extractLibraries(const json& versionJson) {
        std::vector<Library> libraries;
        
        if (versionJson.contains("libraries")) {
            for (const auto& lib : versionJson["libraries"]) {
                Library library;
                if (lib.contains("name")) {
                    library.name = lib["name"].get<std::string>();
                }
                if (lib.contains("downloads") && lib["downloads"].contains("artifact")) {
                    library.path = lib["downloads"]["artifact"]["path"].get<std::string>();
                }
                libraries.push_back(library);
            }
        }
        
        return libraries;
    }
}
