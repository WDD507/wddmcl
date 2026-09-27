#include "auth_service.h"
//#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include "account_manager.h"
#include "../utils/md5_utils.h"
#include "../utils/http_client.h"
#include "../utils/json_parser.h"

// 未来可改为：
const std::string MOJANG_AUTH_URL=
    "https://authserver.mojang.com/authenticate";
const std::string LITTLESKIN_AUTH_URL=
    "https://littleskin.cn/api/yggdrasil";

// Yggdrasil /validate 端点（外置登录令牌校验）
const std::string MOJANG_VALIDATE_URL =
    "https://authserver.mojang.com/validate";
const std::string LITTLESKIN_VALIDATE_URL =
    "https://littleskin.cn/api/yggdrasil/authserver/validate";

// Yggdrasil /refresh 端点
const std::string MOJANG_REFRESH_URL =
    "https://authserver.mojang.com/refresh";
const std::string LITTLESKIN_REFRESH_URL =
    "https://littleskin.cn/api/yggdrasil/authserver/refresh";

// 文件级引用：绑定到 AccountManager 唯一单例，避免每行都写长链式调用
namespace {
    AccountManager& accmgr = AccountManager::instance();
    HttpClient &http_client = HttpClient::instance();
}

bool AuthService::authenticateOffline(const std::string& username) {
    std::cout << "[AuthService] Authenticating offline: " << username << std::endl;

    // DONE(2026/6/13 15:00)(2026/8/23 15:10)
    // 使用MD5算法对用户进行认证
    // 将用户名通过MD5算法生成32位16进制字符串作为accessToken
    std::string md5Hash = MD5Utils::calculateMD5(username);
    // TODO

    std::cout << "[AuthService] Offline authentication successful" << std::endl;
    std::cout << "[AuthService] Generated access token (MD5): " << md5Hash << std::endl;

    Account *acc=accmgr.getAccountFromNamePtr(username);
    if (acc != nullptr) {
        acc->accessToken = md5Hash;
        accmgr.persist();   // 把更新后的 accessToken 写入磁盘
        return true;
    }
    return false;
    /* 旧代码（bug：改了内存对象但未落盘，重启后 token 丢失）
    if (acc != NULL) {
        acc->accessToken = md5Hash;
    }
    return acc != NULL;
    */
}

// ============ Yggdrasil 协议统一认证 ============
// Mojang / LittleSkin / 任意 authlib-injector 兼容站点共用同一套
// 请求格式，仅 URL 不同。返回值表示认证是否成功。
bool AuthService::yggdrasil(
    const std::string &url,
    const std::string &email,
    const std::string &password
) {
    std::cout << "[AuthService] Yggdrasil auth -> " << url
        << " (user: " << email << ")" << std::endl;

    json req;
    req["agent"] = {
        {"name", "Minecraft"},
        {"version", 1}
    };
    req["username"]    = email;
    req["password"]    = password;
    req["requestUser"] = true;

    //static HttpClient http;
    std::string resp = http_client.postRequest(url, req.dump());
    if (resp.empty()) {
        std::cerr << "[AuthService] Yggdrasil auth failed: empty response"
            << std::endl;
        return false;
    }

    json j;
    try {
        j = JsonParser::parseString(resp);
    } catch (const std::exception &e) {
        std::cerr << "[AuthService] Yggdrasil auth parse error: "
            << e.what() << std::endl;
        return false;
    }

    if (j.contains("error")) {
        std::cerr << "[AuthService] Yggdrasil auth error: "
            << j.value("errorMessage", j["error"].get<std::string>())
            << std::endl;
        return false;
    }

    if (!j.contains("accessToken")) {
        std::cerr << "[AuthService] Yggdrasil auth failed: no accessToken"
            << std::endl;
        return false;
    }

    std::string accessToken = j["accessToken"].get<std::string>();
    std::string uuid;
    std::string profileName = email;
    if (j.contains("selectedProfile")) {
        uuid = j["selectedProfile"].value("id", "");
        profileName = j["selectedProfile"].value("name", profileName);
    }

    // 根据 URL 推断账户类型，便于写入对应分组
    AcctType type = AcctType::Other;
    if (url.find("mojang") != std::string::npos) {
        type = AcctType::Mojang;
    }
    if (url.find("littleskin") != std::string::npos) {
        type = AcctType::LittleSkin;
    }

    std::cout << "[AuthService] Yggdrasil auth success, user: "
        << profileName << std::endl;

    // 持久化账户信息（含 uuid 与 accessToken）
    accmgr.addAccount(profileName, type, uuid, accessToken);
    return true;
}

// ============ Mojang 账户认证 ============
bool AuthService::authenticateMojang(
    const std::string &email,
    const std::string &password
) {
    std::cout << "[AuthService] Authenticating Mojang account: "
        << email << std::endl;
    return yggdrasil(
        MOJANG_AUTH_URL,
        email, password
    );
}

// ============ LittleSkin 账户认证 ============
bool AuthService::authenticateLittleSkin(
    const std::string &email,
    const std::string &password
) {
    std::cout << "[AuthService] Authenticating LittleSkin account: "
        << email << std::endl;
    return yggdrasil(
        LITTLESKIN_AUTH_URL,
        email, password
    );//https://littleskin.cn/api/yggdrasilserver/authserver/authenticate
}

/*bool AuthService::authenticateMicrosoft(
    const std::string& email, const std::string& password
) {
    std::cout << "[AuthService] Authenticating Microsoft account..." << std::endl;
    // TODO: 微软OAuth认证
    std::cout << "[AuthService] Sorry, no one wants to follow you to like Microsoft."
        << std::endl;
    return false;
}*/

/*bool AuthService::refreshToken(const std::string& refreshTokenS) {
    std::cout << "[AuthService] Refreshing token..." << std::endl;
    // TODO: 刷新访问令牌
    return true;
}*/
// By TRAE
bool AuthService::refreshToken(Account& acc) {
    std::cout << "[AuthService] Refreshing token..." << std::endl;

    if (acc.type == AcctType::OffLine){
        return true;
    }

    std::string url="";
    if (acc.type == AcctType::Mojang){
        url = MOJANG_REFRESH_URL;
    }else if (acc.type == AcctType::LittleSkin){
        url = LITTLESKIN_REFRESH_URL;
    }else{
        return false;
    }

    nlohmann::json reqBody={
        {"accessToken", acc.accessToken},
        {"requestUser", true}
    };

    std::string respBody;
    int status = http_client.postRequestStatus(url, reqBody.dump());
    if (http_client.maybe_succeed(status)) {
        nlohmann::json resp=nlohmann::json::parse(respBody);
        acc.accessToken = resp["accessToken"].get<std::string>();
        accmgr.persist(); // 持久化到 accounts.json
        return true;
    }
    return false;
}
bool AuthService::refreshToken(const std::string &username) {
    std::cout << "[AuthService] Refreshing token..." << std::endl;
    // DONE(2026/9/27 9:18): 刷新访问令牌
    bool success=true;
    Account cur=*(accmgr.getAccountFromNamePtr(username));
    success &= refreshToken(cur);
    success &= accmgr.addAccount(
        cur.username, cur.type,
        cur.uuid, cur.accessToken
    );
    success &= accmgr.selectAccount(cur.username, cur.type);
    return success;
}
bool AuthService::refreshToken() {
    std::cout << "[AuthService] Refreshing token..." << std::endl;
    // DONE(2026/9/27 9:18): 刷新访问令牌
    bool success=true;
    Account cur=accmgr.getCurrentAccount();
    success &= refreshToken(cur);
    success &= accmgr.addAccount(
        cur.username, cur.type,
        cur.uuid, cur.accessToken
    );
    success &= accmgr.selectAccount(cur.username, cur.type);
    return success;
}

std::string AuthService::getAccessToken() const {
    std::cout << "[AuthService] Getting access token..." << std::endl;
    // DONE(2026/8/23 15:31): 返回访问令牌
    return accmgr.getCurrentAccount().accessToken;
}

bool AuthService::isTokenValid() {
    std::cout << "[AuthService] Checking token validity..." << std::endl;
    Account acc = accmgr.getCurrentAccount();

    // DONE(2026/9/13 10:43)
    // 离线账户：令牌由用户名 MD5 本地生成，永不过期
    if (acc.type == AcctType::OffLine) {
        return !acc.accessToken.empty();
    }

    // 外置登录：调用 Yggdrasil /validate 端点校验
    //   HTTP 204 → 令牌有效
    //   HTTP 403 → 令牌无效/过期
    //   网络异常(状态码 0) → 无法校验，视为无效
    std::string validateUrl;
    switch (acc.type) {
        case AcctType::Mojang:
            validateUrl = MOJANG_VALIDATE_URL;
            break;
        case AcctType::LittleSkin:
            validateUrl = LITTLESKIN_VALIDATE_URL;
            break;
        case AcctType::Microsoft:
            // 微软 OAuth 登录尚未实现，无法校验令牌
            return false;
        case AcctType::Other: default:
            // Other 类型：未知认证源，无法校验
            return false;
    }

    if (acc.accessToken.empty()) {
        return false;
    }

    json req;
    req["accessToken"] = acc.accessToken;
    int status = http_client.postRequestStatus(validateUrl, req.dump());

    bool valid = http_client.maybe_succeed(status);//(status == 204);
    std::cout << "[AuthService] Token validate status: " << status
              << " -> " << (valid ? "valid" : "invalid") << std::endl;
    return valid;
}
