#ifndef VERSION_MANAGER_H
#define VERSION_MANAGER_H

#include <sstream>
#include <string>
#include <vector>
#include "../app.h"
#include "../launcher.h"
#include "../utils/json_parser.h"
#include "../utils/file_system_utils.h"
#include "../utils/json_parser.h"

using json = nlohmann::json;

extern const std::string MOJANG_VER_JSON,MC_RESOURCES_WEBSITE;
// MOJANG_ASSERS_JSON

// 远程版本信息（来自 Mojang API）
struct RemoteVersionInfo {
    std::string id;
    std::string type;
    std::string url;         // 下载 version.json 的地址
    std::string Time;        // ??时间（待确定）
    std::string releaseTime; // 发布时间
    RemoteVersionInfo(
        std::string _id,
        std::string _type,
        std::string _url,
        std::string _time,
        std::string _rlstm
    );
    RemoteVersionInfo();
    ~RemoteVersionInfo() = default;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        RemoteVersionInfo, id, type, url, Time, releaseTime
    )
};
// 本地版本信息（来自 .minecraft/versions/x.x.x/x.x.x.json）
struct VersionInfo {
    std::string id;                     // 版本ID (如 "1.20.1")
    std::string clientver;
    std::string type;                   // 类型 (release/snapshot)
    std::string mainClass;              // 主入口点
    std::string javaVersion;            // 所需Java版本
    std::vector<std::string> libraries; // 依赖库
    bool user=false;
    VersionInfo(
        std::string _id,
        std::string _cliver,
        std::string _type,
        std::string _maincls,
        std::string _javaver,
        std::vector<std::string> _libs,
        bool _user = false
    );
    VersionInfo(const RemoteVersionInfo &ver, bool download=false);
    VersionInfo();
    ~VersionInfo() = default;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        VersionInfo, id, clientver, type, mainClass,
        javaVersion, libraries, user
    )
};

class VersionManager {
public:
    static VersionManager &instance();

    static std::vector<RemoteVersionInfo> remoteVer;

    // 获取可用版本列表
    std::vector<std::string> getAvailableVersions();

    json getVersionsJSON(
        const std::string &path=vers_mainf_dwnld_path
    );
    std::pair<std::string, std::string> getLatestVersion(
        const std::string &path=vers_mainf_dwnld_path
    );
    std::vector<RemoteVersionInfo> getVersionsFromMojang(
        const std::string &path=vers_mainf_dwnld_path
    );

    //
    //bool downloadAsserts(const std::string &url, const std::string &version);
    bool downloadAssets(const json &verj, const std::string &mcdir);

    // 下载指定版本
    bool downloadVersionfromURL(
        const std::string &url,
        const std::string &version
    );
    bool downloadVersion(const std::string& version);
    
    // 检查版本是否已安装
    bool isVersionInstalled(const std::string& version);
    
    // 获取版本信息
    VersionInfo getVersionInfo(const std::string& version);
    
    // 删除版本
    bool removeVersion(const std::string& version);

    //
    bool shouldIncludeLibrary(
        const json &libj,
        const std::string& osName,
        const std::string& osVersion,
        const std::map<std::string, bool>& features = {}
    );
};

#endif // VERSION_MANAGER_H

