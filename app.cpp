//#include <iostream>
#include "app.h"
#include <cstddef>
//#include <cstdint>
#include <cstdlib>
#include <string>
//#include "launcher.h"
#include "utils/json_parser.h"
#include "utils/process_utils.h"
#include "utils/version_utils.h"
#include "utils/http_client.h"
#include "utils/file_system_utils.h"
#ifdef _WIN32
#include <windows.h>
#include <minwindef.h>
//#include <sysinfoapi.h>
#endif
using namespace std;
using json = nlohmann::json;

const string launcher_id_name="wdds-minecraft-launcher";
const Version launcher_ver="0.0.5.1"; // 在此处输入软件版本
// 注意：无论是添加什么类型的版本，那么在该版本发布时都应该将最后面的编译次数加一。
string launcher_config_file="launcher_config.json";
string account_config_file="";
string game_config_file="game_config.json";
string vers_mainf_dwnld_path=
#ifdef _WIN32
        ".\\version_mainfest.json"
#else
        "./version_mainfest.json"
#endif
;
vector<string> args(0);

/*App::App(Launcher _lc) {
    this->launcher = _lc;
}
App::App(bool MCLinit, string MCLstatus) {
    *this = App(Launcher(MCLinit, MCLstatus));
}*/

// ==================== 应用基本信息 ====================

Version App::AppVersion=launcher_ver;

// 应用名称（如需修改请改这里）
static const string APP_NAME_STR = "WDD's Minecraft Launcher";

// 更新服务器地址（请替换为你自己的服务器URL）
// 版本检查接口：应返回JSON，包含 "latest_version" 字段
// 如 {"latest_version":"0.0.6.0"}
static const string UPDATE_CHECK_URL =
"https://www.wddwxy.blog.cn/programme/applications/wddmcl/version.json";
// 更新包下载基础URL（按平台拼接文件名）
static const string UPDATE_DOWNLOAD_URL =
"https://www.wddwxy.blog.cn/programme/applications/wddmcl/downloads/";

string App::getAppName() {
    return APP_NAME_STR;
}

string App::getUpdateCheckUrl() {
    return UPDATE_CHECK_URL;
}

string App::getUpdateDownloadUrl() {
    return UPDATE_DOWNLOAD_URL;
}

// ==================== 更新检查 ====================

int App::checkForUpdate(Version& latestVersion) {
    cout << "[App] Checking for update..." << endl;
    cout << "[App] Current version: " << AppVersion.ver_to_string() << endl;

    // 1. 从服务器获取最新版本信息
    HttpClient http;
    string response = http.getRequest(UPDATE_CHECK_URL);
    if (response.empty()) {
        cerr << "[App] Failed to fetch version info from server" << endl;
        return -1;
    }

    // 2. 解析JSON响应，期望格式: {"latest_version":"x.y.z.w"}
    try {
        json j = JsonParser::parseString(response);
        string latest = j.value("latest_version", "");
        if (latest.empty()) {
            cerr << "[App] No 'latest_version' field in server response" << endl;
            return -1;
        }
        latestVersion = latest;
        cout << "[App] Latest version on server: " << latest << endl;

        // 3. 比较版本号
        if (latestVersion > AppVersion) {
            cout << "[App] New version available!" << endl;
            return 1;
        }
        cout << "[App] Already up to date." << endl;
        return 0;
    } catch (const exception& e) {
        cerr << "[App] Failed to parse version JSON: " << e.what() << endl;
        return -1;
    }
}

// ==================== 从本地文件更新 ====================

int App::AppUpdateFromFile(const string &NewVerFileName) {
    string FullName = NewVerFileName;
#ifdef _WIN32
    FullName += ".exe";
    // 检查更新文件是否存在
    if (!FileSystemUtils::fileExists(FullName)) {
        cerr << "[App] Update file not found: " << FullName << endl;
        return -1;
    }
    cout << "[App] Launching update executable: " << FullName << endl;
    // 启动新版本程序（非阻塞）
    ProcessUtils::launchInBackground(FullName);
#elif defined(__linux__) || defined(WDDWXY_PASS)
    FullName += ".tar.gz";
    if (!FileSystemUtils::fileExists(FullName)) {
        cerr << "[App] Update file not found: " << FullName << endl;
        return -1;
    }
    cout << "[App] Extracting update archive: " << FullName << endl;
    // 解压 tar.gz 到当前目录
    string cmd = "tar -xzf \"" + FullName + "\" -C ./";
    int ret = system(cmd.c_str());
    if (ret != 0) {
        cerr << "[App] Failed to extract update archive (exit code: " << ret << ")" << endl;
        return -1;
    }
    // 启动解压后的新版本
    string newExe = "./wddmcl";
    if (FileSystemUtils::fileExists(newExe)) {
        cout << "[App] Launching new version: " << newExe << endl;
        ProcessUtils::launchInBackground(newExe);
    } else {
        cerr << "[App] New executable not found after extraction: " << newExe << endl;
        return -1;
    }
#elif defined(WDDWXY_PASS)
    // TODO: 纯血鸿蒙（HarmonyOS NEXT，基于HongMeng微内核，不兼容Linux）
    // 注意：兼容Linux的鸿蒙版本（OpenHarmony标准版）走 __linux__ 分支
    // 包格式与更新机制未定，待确定后再补充
    FullName += ".NULL";
    cerr << "[App] Update not supported on HarmonyOS NEXT yet." << endl;
    return -1;
#else
    cerr << "[App] Unsupported platform for update." << endl;
    return -1;
#endif
    // 退出当前进程，让新版本接管
    exit(0);
    return 0;
}

// ==================== 从网站更新 ====================

int App::AppUpdateFromWebsite() {
    cout << "[App] Updating from website..." << endl;

    // 1. 检查是否有新版本
    Version latest;
    int checkResult = checkForUpdate(latest);
    if (checkResult <= 0) {
        // 0=已是最新, -1=检查失败
        cout << "[App] No update needed or check failed." << endl;
        return checkResult;
    }

    // 2. 按平台构造下载URL和本地保存路径
    string downloadUrl = UPDATE_DOWNLOAD_URL;
    string localPath;
#ifdef _WIN32
    downloadUrl += "wddmcl-windows.exe";
    localPath = ".\\newupdate.exe";
#elif defined(__linux__)
    downloadUrl += "wddmcl-linux.tar.gz";
    localPath = "./newupdate.tar.gz";
#elif defined(WDDWXY_PASS)
    // TODO: 纯血鸿蒙包格式未定（兼容Linux的鸿蒙走 __linux__ 分支）
    cerr << "[App] Download not supported on HarmonyOS NEXT yet." << endl;
    return -1;
#else
    cerr << "[App] Unsupported platform for download." << endl;
    return -1;
#endif

    // 3. 下载更新文件
    cout << "[App] Downloading update from: " << downloadUrl << endl;
    cout << "[App] Saving to: " << localPath << endl;
    HttpClient http;
    if (!http.downloadFile(downloadUrl, localPath)) {
        cerr << "[App] Failed to download update." << endl;
        return -1;
    }

    // 4. 验证文件下载成功
    if (!FileSystemUtils::fileExists(localPath)) {
        cerr << "[App] Downloaded file not found on disk." << endl;
        return -1;
    }
    long long fileSize = FileSystemUtils::getFileSize(localPath);
    cout << "[App] Update downloaded successfully (" << fileSize << " bytes)." << endl;

    // 5. 调用 AppUpdateFromFile 启动新版本（去掉扩展名）
    string baseName =
#ifdef _WIN32
        ".\\newupdate";
#else
        "./newupdate";
#endif
    return AppUpdateFromFile(baseName);
}


// 返回当前操作系统的名称（"windows" / "linux" / "osx"）
std::string App::getCurrentOSName() {
#if defined(_WIN32)
    return "windows";
#elif defined(__linux__)
    return "linux";
#elif defined(__APPLE__)
    return "osx";
#else
    return "unknown";
#endif
}
// 返回当前操作系统的版本号字符串（用于 os.version 正则匹配）
// Windows 返回 "10.0"（Win10/11）；beta 阶段可硬编码，正式版用 GetVersionEx 动态获取
std::string App::getCurrentOSVersion() {
#if defined(_WIN32)
    return "10.0";//GetVersionEx;
#else
    return "";
#endif
}
size_t App::getCurrentOSBitNum() {
    return sizeof(void*) * 8;
}
string App::getCurrentOSBit() {
    return to_string(getCurrentOSBitNum());;
}
