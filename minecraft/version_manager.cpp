#include "version_manager.h"
#include <float.h>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <regex>
#include "../utils/file_system_utils.h"
#include "../utils/zip_utils.h"
#include "../utils/http_client.h"
//#include <algorithm>

const std::string MOJANG_VER_JSON=
    "https://piston-meta.mojang.com/mc/game/version_manifest.json";
const std::string MC_RESOURCES_WEBSITE=
    "https://resources.download.minecraft.net/";

namespace {
    HttpClient &http_client=HttpClient::instance();
    VersionManager &verm=VersionManager::instance();
}

RemoteVersionInfo::RemoteVersionInfo(
    std::string _id,
    std::string _type,
    std::string _url,
    std::string _time,
    std::string _rlstm
) {
    this->id = _id;
    this->type = _type;
    this->url = _url;
    this->Time = _time;
    this->releaseTime = _rlstm;
}
RemoteVersionInfo::RemoteVersionInfo() {
    *this = RemoteVersionInfo(
        "", "", "", "", ""
    );
}

VersionInfo::VersionInfo(
    std::string _id,
    std::string _cliver,
    std::string _type,
    std::string _maincls,
    std::string _javaver,
    std::vector<std::string> _libs,
    bool _user
) {
    this->id = _id;
    this->clientver = _cliver;
    this->type = _type;
    this->mainClass = _maincls;
    this->javaVersion = _javaver;
    this->libraries = _libs;
    this->user = _user;
}
VersionInfo::VersionInfo(
    const RemoteVersionInfo &ver, bool download
) {
    this->id = this->clientver = ver.id;
    this->type = ver.type;
    if (download) {
        //HttpClient http_client;
        //http_client.downloadFile(ver.url, "");
        //static VersionManager verm;
        verm.downloadVersion(ver.id);
        *this = verm.getVersionInfo(ver.id);
    }
}
VersionInfo::VersionInfo() {
    *this = VersionInfo(
        "", "", "", "", "",
        {}, false
    );
}

std::vector<RemoteVersionInfo> VersionManager::remoteVer={};

std::vector<std::string> VersionManager::getAvailableVersions() {
    std::cout << "[VersionManager] Getting available versions..." << std::endl;
    
    // 从文件系统扫描已安装的版本
    return FileSystemUtils::getInstalledVersions();
}

json VersionManager::getVersionsJSON(const std::string &path) {
    return (
        (path == "") ?
        JsonParser::parseString(
            "{\"latest\": {}, \"versions\": []}"
        ) : 
        JsonParser::parseFile(path)
    );
}
std::pair<std::string, std::string>
VersionManager::getLatestVersion(const std::string &path) {
    std::pair<std::string, std::string> result(
        "1.21.11", "1.21.11-rc3"
    );
    //HttpClient http_client;
    http_client.downloadFile(
        MOJANG_VER_JSON,
        path
    );
    json j=getVersionsJSON(path)["latest"];
    result.first = j["release"].get<std::string>();
    result.second = j["snapshot"].get<std::string>();
    return result;
}
std::vector<RemoteVersionInfo> VersionManager::getVersionsFromMojang(
    const std::string &path
) {
    //HttpClient http_client;
    http_client.downloadFile(
        MOJANG_VER_JSON,
        path
    );
    json j=getVersionsJSON(path);
    return remoteVer = j["versions"].get<std::vector<RemoteVersionInfo> >();
}

/*bool VersionManager::downloadAsserts(
    const std::string &url,
    const std::string &version
) {
    http_client.downloadFile(url, version);
}*/
bool VersionManager::downloadAssets(
    const json &verj,
    const std::string &mcdir
) {
    bool success=true;
    std::string indexId=verj["assetIndex"]["id"].get<std::string>();
    std::string indexURL=verj["assetIndex"]["url"].get<std::string>();
    std::string indexJSON=mcdir+"/assets/indexes/"+indexId+".json";
    success &= http_client.downloadFile(indexURL, indexJSON);
    json assetj=JsonParser::parseFile(indexJSON);
    for (
        auto& [path, obj]
            : assetj["objects"].items()
    ) {
        std::cout << "[VersionManager] Downloading Asset: " << path << std::endl;
        std::string hash=obj["hash"].get<std::string>();
        std::string subhash=hash.substr(0, 2);
        std::string mcdirpath=mcdir+"/assets/objects/"+subhash+"/"+hash;
        if (FileSystemUtils::fileExists(mcdirpath)) {
            continue;
        }
        success &= http_client.downloadFile(
            MC_RESOURCES_WEBSITE/* + "/"*/ + subhash + "/" + hash,
            mcdirpath
        );
    }
    return success;
}

bool VersionManager::downloadVersionfromURL(
    const std::string &url,
    const std::string &version
) {
    bool success=true;
    //static HttpClient http_client;
    std::string versionDir=
        FileSystemUtils::getVersionDirectory(version);
    success &= http_client.downloadFile(
        url,
        versionDir + FileSystemUtils::getSlash() +
        version + ".json"
    );
    json j=JsonParser::parseFile(
        versionDir + FileSystemUtils::getSlash() +
        version + ".json"
    );
    success |= http_client.downloadFile(
        j["downloads"]["client"]["url"],
        versionDir + FileSystemUtils::getSlash() +
        version + ".jar"
    );
    for (json lib : j["libraries"]) {
/*#ifndef MACOS
        if (
            lib.contains("rules") &&
            lib["rules"][0].contains("os") &&
            lib["rules"][0]["os"].get<std::string>() == "osx"
        ){
            continue;
        }
#endif
#ifndef __linux__
        if (
            lib.contains("rules") &&
            lib["rules"][0].contains("os") &&
            lib["rules"][0]["os"].get<std::string>() == "linux"
        ){
            continue;
        }
#endif*/
        if (shouldIncludeLibrary(
            lib,
            App::getCurrentOSName(),
            App::getCurrentOSVersion()
        )) {
            if (lib["downloads"].contains("artifact")) {
                success |= http_client.downloadFile(
                    lib["downloads"]["artifact"]["url"].get<std::string>(),
                    FileSystemUtils::getLibrariesDirectory() +
                    lib["downloads"]["artifact"]["path"].get<std::string>()
                );
            }
            if (lib["downloads"].contains("classifiers")) {
                std::string natives="",savepath="";
                if (lib.contains("natives")) {
                    natives = lib["natives"][App::getCurrentOSName()]
                        .get<std::string>();
                }else{
                    continue;//natives = App::getCurrentOSName();
                }
                // 替换 ${arch} 占位符：64 位 -> "64"，32 位 -> "32"
                // Windows: sizeof(void*)==8 表示 64 位
                std::string arch=App::getCurrentOSBit();
                size_t pos=natives.find("${arch}");
                if (pos != std::string::npos) {
                    natives.replace(pos, 7, arch);
                }
                // 确认该 classifier 存在
                if(!lib["downloads"]["classifiers"].contains(natives)) {
                    continue;
                }
                savepath = FileSystemUtils::getLibrariesDirectory() +
                        lib["downloads"]["classifiers"][natives]["path"]
                            .get<std::string>();
                success |= http_client.downloadFile(
                    lib["downloads"]["classifiers"][natives]["url"]
                        .get<std::string>(),
                    savepath
                );
                ZipUtils::extractAllExclude(
                    savepath,
                    FileSystemUtils::getVersionDirectory(version)
                        + "/natives",
                    "META-INF/"
                );
            }
        }
    }
    success &= downloadAssets(
        j, FileSystemUtils::getMinecraftDirectory()
    );
    return success;
}
bool VersionManager::downloadVersion(const std::string& version) {
    std::cout << "[VersionManager] Downloading version: " << version << std::endl;
    
    // DONE1(2026/8/22 17:04): 需要HTTP客户端下载版本文件
    // 1. 从 Mojang API 获取版本清单
    // 2. 下载 version.json
    // 3. 下载游戏jar和库文件
    // 4. 下载资源文件
    
    /*std::cout << "[VersionManager] WARNING: HTTP functionality not implemented yet"
        << std::endl;
    std::cout << "[VersionManager] Please manually place version files in:" << std::endl;
    std::cout << "[VersionManager] "
        << FileSystemUtils::getVersionDirectory(version) << std::endl;*/
    //return false;  // 暂时返回false，表示未完成

    std::string url="";
    if (remoteVer.empty()) {
        /*remoteVer = */getVersionsFromMojang();
    }
    for (const RemoteVersionInfo &rvi : remoteVer) {
        if (rvi.id == version) {
            url = rvi.url;
            break;
        }
    }
    return (!(url.empty()) ? downloadVersionfromURL(url, version) : false);
}

bool VersionManager::isVersionInstalled(const std::string& version) {
    std::cout << "[VersionManager] Checking if version is installed: "
        << version << std::endl;
    return FileSystemUtils::isVersionInstalled(version);
}

VersionInfo VersionManager::getVersionInfo(const std::string& version) {
    std::cout << "[VersionManager] Getting version info: " << version << std::endl;
    
    VersionInfo info;
    info.id = version;
    
    // 尝试从 version.json 读取信息
    std::string versionJsonPath = FileSystemUtils::getVersionDirectory(version) + "/" + version + ".json";
    
    if (FileSystemUtils::fileExists(versionJsonPath)) {
        try {
            json jInfo = JsonParser::parseFile(versionJsonPath);
            
            info.id = jInfo.value("id", version);
            info.type = jInfo.value("type", "release");
            info.mainClass = jInfo.value("mainClass", ""); // releaseTime
            
            // 获取Java版本要求
            if (jInfo.contains("javaVersion") && jInfo["javaVersion"].contains("majorVersion")) {
                info.javaVersion = std::to_string(jInfo["javaVersion"]["majorVersion"].get<int>());
            } else {
                // 默认根据Minecraft版本推断Java版本
                if (version >= "1.18") {
                    info.javaVersion = "17";
                } else if (version >= "1.17") {
                    info.javaVersion = "16";
                } else {
                    info.javaVersion = "8";
                }
            }
            
            // 提取库列表
            if (jInfo.contains("libraries")) {
                for (const auto& lib : jInfo["libraries"]) {
                    if (lib.contains("name")) {
                        info.libraries.push_back(lib["name"].get<std::string>());
                    }
                }
            }
            
            std::cout << "[VersionManager] Version info loaded from JSON" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[VersionManager] Failed to parse version JSON: " << e.what() << std::endl;
            // 使用默认值
        }
    } else {
        std::cout << "[VersionManager] version.json not found, using defaults" << std::endl;
        
        // 设置默认值
        info.type = "release";
        if (version >= "1.18") {
            info.javaVersion = "17";
        } else if (version >= "1.17") {
            info.javaVersion = "16";
        } else {
            info.javaVersion = "8";
        }
    }
    
    return info;
}

bool VersionManager::removeVersion(const std::string& version) {
    std::cout << "[VersionManager] Removing version: " << version << std::endl;
    
    if (!isVersionInstalled(version)) {
        std::cerr << "[VersionManager] Version not installed: " << version << std::endl;
        return false;
    }
    
    std::string versionDir = FileSystemUtils::getVersionDirectory(version);
    
    if (FileSystemUtils::deleteDirectory(versionDir)) {
        std::cout << "[VersionManager] Version removed successfully" << std::endl;
        return true;
    } else {
        std::cerr << "[VersionManager] Failed to remove version directory" << std::endl;
        return false;
    }
}

/**
 * By TRAE
 * @brief 判断某个 library 是否应该被包含（下载并加入 classpath）
 * 
 * 评估逻辑（对齐官方启动器）：
 *   - 无 rules → 默认包含
 *   - 有 rules → 顺序遍历，取最后一条"匹配当前环境"的 rule 的 action
 *   - 无任何 rule 匹配 → 默认不包含
 * 
 * @param lib         version.json 中某个 library 的 JSON 对象
 * @param osName      当前 OS 名称（"windows"/"linux"/"osx"）
 * @param osVersion   当前 OS 版本号字符串
 * @param features    当前启动特性（如 {{"is_demo_user", false}}），可为空
 * @return true 表示该库应被包含
 */
bool VersionManager::shouldIncludeLibrary(
    const json &libj,
    const std::string& osName,
    const std::string& osVersion,
    const std::map<std::string, bool>& features
) {
    /*if (libj.contains("rules")) {
        for (const json &rul : libj["rules"].get<std::vector<json> >()) {
#ifndef MACOS
            if(
                rul["os"].get<std::string>() == "osx"
            ){
                //
            }
#endif
#ifndef __linux__
            if(
                rul["os"].get<std::string>() == "linux"
            ){
                //
            }
#endif
        }
    }
    return true;*/
    // 无 rules → 默认包含
    if (!libj.contains("rules") || libj["rules"].empty()) {
        return true;
    }

    bool allowed = false;  // 有 rules 时默认不包含，需被 allow 覆盖

    for (const json& rule : libj["rules"]) {
        bool matches = true;

        // ---- 检查 os 条件 ----
        if (matches && rule.contains("os")) {
            const auto& os = rule["os"];

            // os.name 必须等于当前 OS
            if (os.contains("name")) {
                if (os["name"].get<std::string>() != osName) {
                    matches = false;
                }
            }

            // os.version 必须正则匹配当前 OS 版本
            if (matches && os.contains("version")) {
                try {
                    std::regex verRegex(os["version"].get<std::string>());
                    if (!std::regex_search(osVersion, verRegex)) {
                        matches = false;
                    }
                } catch (const std::regex_error&) {
                    // 正则表达式无效 → 视为不匹配
                    matches = false;
                }
            }
        }
        // ---- 检查 features 条件 ----
        if (matches && rule.contains("features")) {
            for (
                const auto& [
                    featName,
                    featRequired
                ] : rule["features"].items()
            ) {
                // 查找该特性在当前环境中的值；未提供的特性默认 false
                bool currentValue = false;
                auto it = features.find(featName);
                if (it != features.end()) {
                    currentValue = it->second;
                }

                // 要求的特性值与当前值不符 → 不匹配
                if (featRequired.get<bool>() != currentValue) {
                    matches = false;
                    break;
                }
            }
        }
        // ---- 该 rule 匹配当前环境 → 更新结果 ----
        if (matches) {
            allowed = (rule["action"].get<std::string>() == "allow");
            // 注意：不 break，继续看后续 rule，取最后一个匹配的 action
        }
        // 可能还有更多未检查的条件
    }
    return allowed;
}
