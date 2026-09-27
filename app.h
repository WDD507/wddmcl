#include <cstddef>
#include <cstdint>
#ifndef APP_H
#define APP_H "app.h"
#include <iostream>
#include <string>
#include <vector>
//#include "launcher.h"
#include "utils/version_utils.h"
#include "utils/http_client.h"
#include "utils/file_system_utils.h"
//#include "config/config_manager.h"

extern const std::string launcher_id_name;
extern const Version launcher_ver;
extern/* const*/ std::string launcher_config_file;
extern std::string account_config_file;
extern std::string game_config_file;
extern std::string vers_mainf_dwnld_path;
extern std::vector<std::string> args;

//using AppSettings = ConfigManager;
class App {
public:
    /*Launcher launcher;
    App(Launcher _lc=Launcher());
    App(bool MCLinit=false, std::string MCLstatus="NotReady");
    ~App() = default;*/
    static Version AppVersion;
    // 应用名称
    static std::string getAppName();
    // 更新服务器地址（请根据实际情况修改为你的服务器URL）
    static std::string getUpdateCheckUrl();
    static std::string getUpdateDownloadUrl();
    // 检查更新：比较本地版本与服务器最新版本
    // 返回值: 1=有更新, 0=已是最新, -1=检查失败
    // latestVersion 输出服务器返回的最新版本号
    static int checkForUpdate(Version& latestVersion);
    static int AppUpdateFromFile(
        const std::string &NewVerFileName=
#ifdef _WIN32
        ".\\newupdate"
#else
        "./newupdate"
#endif
    ); // 注意这里的更新文件所用的名称不应包括后缀名。
    static int AppUpdateFromWebsite();

    //
    static std::string getCurrentOSName();
    static std::string getCurrentOSVersion();
    static size_t getCurrentOSBitNum();
    static std::string getCurrentOSBit();
};
#endif /* app.h */
