#include "account_manager.h"
#include <algorithm>
//#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
/* yvals_core.h 是 MSVC 特有头，MinGW 下不存在，注释掉
#ifdef _MSC_VER
#include <yvals_core.h>
#endif*/
#include "../utils/json_parser.h"
using json = nlohmann::json;

// DONE(2026/9/5 14:38): AI fixed.

// Meyers' Singleton：C++11 起函数内 static 局部变量初始化线程安全
AccountManager& AccountManager::instance() {
    static AccountManager inst;
    return inst;
}

Account::Account(
    const std::string &_usrnm,
    const std::string &_uuid,
    const std::string &_acstkn,
    const AcctType &_type,
    bool _vld
) {
    this->username = _usrnm;
    this->uuid = _uuid;
    this->accessToken = _acstkn;
    this->type = _type;
    this->isValid = _vld;
}
Account::Account(
    const std::string &_usrnm,
    const std::string &_uuid,
    const std::string &_acstkn,
    int _type,
    bool _vld
) {
    *this = Account(
        _usrnm, _uuid, _acstkn,
        static_cast<AcctType>(_type), _vld
    );
}
Account::Account() {
    *this = Account("", "", "",
        AcctType::OffLine, false);
}

const Account ACC_NOT_FOUND=Account(
    "Not Found", "notfound",
    "notfound", 0, false
);

bool operator==(const Account &a, const Account &b) {
    return (
        (a.username == b.username) &&
        (a.uuid == b.uuid) &&
        (a.accessToken == b.accessToken) &&
        (a.type == b.type) &&
        (!(a.isValid ^ b.isValid))
    );
}

AcctType AccountManager::acctypels[5]={
    AcctType::OffLine,
    AcctType::Mojang,
    AcctType::LittleSkin,
    AcctType::Other,
    AcctType::Microsoft
};
std::string AccountManager::AcctTypeToStr(const AcctType &type) {
    std::string key="";
    switch (type) {
        case AcctType::OffLine:         key = "Offline";    break;
        case AcctType::Mojang:          key = "Mojang";     break;
        case AcctType::LittleSkin:      key = "LittleSkin"; break;
        case AcctType::Microsoft:       key = "Microsoft";  break;
        case AcctType::Other: default:  key = "Other";      break;
    }
    return key;
}

int AccountManager::reloadAccount() {
    // 强制从磁盘重新加载所有账户信息
    loadFromDisk();
    listOfflineAccounts();
    listMojangAccounts();
    listLittleskinAccounts();
    return 0;
}

bool AccountManager::addAccount(
    const std::string& username,
    const AcctType& type
) {
    std::cout << "[AccountManager] Adding account: "
        << username << " (" << int(type) << ")" << std::endl;
    // DONE(2026/6/7 ??:??): 保存账户信息
    json j=JsonParser::parseFile(account_config_file),tmp={};
    tmp["username"] = username; tmp["type"] = int(type);
    j["Account"][AcctTypeToStr(type)].push_back(tmp);
    JsonParser::saveToFile(j, account_config_file);
    return true;
}

// 带 uuid 与 accessToken 的重载：按 type 分发到对应数组并写入完整字段
bool AccountManager::addAccount(
    const std::string& username,
    const AcctType& type,
    const std::string& uuid,
    const std::string& accessToken
) {
    std::cout << "[AccountManager] Adding account (with token): "
        << username << " (" << int(type) << ")" << std::endl;
    ensureLoaded();
    auto it = findAccount(username);
    if (it != m_accounts.end()) {
        // 已存在：更新 token/uuid/type
        (*it)->uuid        = uuid;
        (*it)->accessToken = accessToken;
        (*it)->type        = type;
        (*it)->isValid     = true;
    } else {
        m_accounts.push_back(std::make_unique<Account>(
            username, uuid, accessToken, type, true
        ));
    }
    saveToDisk();
    return true;
    /* 旧代码（无状态版，每次直接写文件）
    json j = JsonParser::parseFile(account_config_file);
    std::string key=AcctTypeToStr(type);
    json &arr = j["Account"][key];
    bool found = false;
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i].value("username", "") == username) {
            arr[i]["uuid"]        = uuid;
            arr[i]["accessToken"] = accessToken;
            arr[i]["type"]        = int(type);
            arr[i]["isValid"]     = true;
            found = true;
            break;
        }
    }
    if (!found) {
        json tmp={};
        tmp["username"]     = username;
        tmp["uuid"]         = uuid;
        tmp["accessToken"]  = accessToken;
        tmp["type"]         = int(type);
        tmp["isValid"]      = true;
        arr.push_back(tmp);
    }
    JsonParser::saveToFile(j, account_config_file);
    return true;
    */
}

bool AccountManager::selectAccount(const std::string& username, const AcctType &type) {
    std::cout << "[AccountManager] Selecting account: " << username
        << std::endl;
    // DONE(2026/6/7 ??:??)(2026/6/13 15:39): 选择当前账户（内存态缓存版本）
    ensureLoaded();
    auto it = findAccount(username);
    if (it != m_accounts.end() && (*it)->type == type) {
        m_currentAccount = username;
        saveToDisk();
        return true;
    }
    return false;
    /* 旧代码（无状态版：每次 parseFile + saveToFile，且只在对应 type 组里找）
    json j=JsonParser::parseFile(account_config_file);
    std::string key=AcctTypeToStr(type);
    for(size_t i=0;i<j["Account"][key].size();i++){
        if (j["Account"][key][i]["username"] == username) {
            j["current_account"] = username;
            JsonParser::saveToFile(j, account_config_file);
            return true;
        }
    }
    JsonParser::saveToFile(j, account_config_file);
    return false;
    */
}

Account AccountManager::getCurrentAccount() {
    // DONE: 内存态缓存版本
    ensureLoaded();
    if (m_currentAccount.empty()) {
        return ACC_NOT_FOUND;
    }
    return getAccountFromName(m_currentAccount);
    /* 旧代码（无状态版，每次 parseFile）
    json j=JsonParser::parseFile(account_config_file);
    return (
        (j.contains("current_account")) ?
        (
            getAccountFromName(
                j["current_account"].get<std::string>()
            )
        ) :
        ACC_NOT_FOUND
    );
    */
}
std::string AccountManager::getCurrentAccountStr() const {
    std::cout << "[AccountManager] Getting current account..." << std::endl;
    // DONE(2026/6/7 ??:??): 返回当前账户名（内存态缓存版本）
    // 注意：const 方法不调 ensureLoaded；若尚未加载，返回空串/Not Found
    return m_loaded ?
        (m_currentAccount.empty() ? "Not Found" : m_currentAccount) :
        "Not Found";
    /* 旧代码（无状态版，每次 parseFile）
    json j=JsonParser::parseFile(account_config_file);
    return std::string(
        (
            j.contains("current_account")
        ) ? (
            j["current_account"].get<std::string>()
        ) : "Not Found"
    );
    */
}

std::vector<std::string> AccountManager::listTypeAccounts(
    const AcctType &Type
) {
    // DONE(2026/8/24 18:52): 内存态缓存版本
    ensureLoaded();
    std::vector<std::string> result;
    for (const auto &acc : m_accounts) {
        if (acc->type == Type) {
            result.push_back(acc->username);
        }
    }
    return result;
    /* 旧代码（无状态版，每次 parseFile）
    std::string StrType=AcctTypeToStr(Type);
    std::vector<std::string> result(0);
    json j=JsonParser::parseFile(account_config_file);
    for(size_t i=0;i<j["Account"][StrType].size();i++){
        result.push_back(
            j["Account"][StrType][i]["username"].get<std::string>()
        );
    }
    return result;
    */
}
std::vector<std::string> AccountManager::listOfflineAccounts() {
    std::cout << "[AccountManager] Listing offline accounts..."
        << std::endl;
    // DONE(2026/6/7 20:12)(2026/8/24 18:53): 返回离线账户列表
    return OfflineList = listTypeAccounts(AcctType::OffLine);
}
std::vector<std::string> AccountManager::listMojangAccounts() {
    std::cout << "[AccountManager] Listing Mojang accounts..."
        << std::endl;
    // DONE(2026/8/24 18:54)
    return MojangList = listTypeAccounts(AcctType::Mojang);
}
std::vector<std::string> AccountManager::listLittleskinAccounts() {
    std::cout << "[AccountManager] Listing LittleSkin accounts..."
        << std::endl;
    // DONE(2026/6/7 20:14)(2026/8/24 18:54): 返回Littleskin账户列表
    return LittleskinList = listTypeAccounts(AcctType::LittleSkin);
}
std::vector<std::string> AccountManager::listAccounts() {
    std::cout << "[AccountManager] Listing accounts..." << std::endl;
    // DONE(2026/6/7 20:15): 返回所有账户列表
    //std::vector<std::string> AccList={};
    /*std::vector<std::string> OfflineList=listOfflineAccounts();
    std::vector<std::string> MojangList=listMojangAccounts();
    std::vector<std::string> LittleskinList=listLittleskinAccounts();*/
    AccList = OfflineList;
    AccList.insert(
        AccList.end(),
        MojangList.begin(), MojangList.end()
    );
    AccList.insert(
        AccList.end(),
        LittleskinList.begin(), LittleskinList.end()
    );
    return AccList;
}

std::vector<Account> AccountManager::getStructureAccounts() {
    // DONE: 内存态缓存版本（从 m_accounts 拷贝出值，保持返回类型不变）
    ensureLoaded();
    StructureAccounts.clear();
    for (const auto &acc : m_accounts) {
        StructureAccounts.push_back(*acc);
    }
    return StructureAccounts;
    /* 旧代码（无状态版，每次 parseFile 反序列化）
    json j=JsonParser::parseFile(account_config_file);
    std::vector<Account> &result=StructureAccounts;
    if (j.contains("Account") && j["Account"].is_object()) {
        for (auto it = j["Account"].begin();
            it != j["Account"].end(); ++it) {
            if (it.value().is_array()) {
                std::vector<Account> part =
                    it.value().get<std::vector<Account>>();
                result.insert(
                    result.end(),
                    part.begin(), part.end()
                );
            }
        }
    }
    return result;
    */
}
Account AccountManager::getAccountFromName(
    const std::string &username
) {
    // DONE: 内存态缓存版本
    ensureLoaded();
    auto it = findAccount(username);
    if (it != m_accounts.end()) {
        return **it;
    }
    return ACC_NOT_FOUND;
    /* 旧代码（无状态版，依赖 getStructureAccounts 的临时 vector）
    std::vector<Account> accounts=getStructureAccounts();
    for (const Account &acc : accounts) {
        if (acc.username == username) {
            return acc;
            //break;
        }
    }
    return ACC_NOT_FOUND;
    */
}
Account *AccountManager::getAccountFromNamePtr(
    const std::string &username
) {
    // DONE: 内存态缓存版本
    // 返回 m_accounts 中元素的裸指针，对象地址在 vector 扩容时不变（unique_ptr）
    ensureLoaded();
    auto it = findAccount(username);
    if (it != m_accounts.end()) {
        return it->get();
    }
    return nullptr;
    /* 旧代码（bug：返回临时 vector 元素的指针，函数返回后悬空 → UB）
    std::vector<Account> accounts=getStructureAccounts();
    for (Account &acc : accounts) {
        if (acc.username == username) {
            return &acc;
            //break;
        }
    }
    return NULL;
    */
}
/*Account &AccountManager::getAccountFromNameRef(
    const std::string &username
) const {
    std::vector<Account> accounts=getStructureAccounts();
    for (Account &acc : accounts) {
        if (acc.username == username) {
            return acc;
            //break;
        }
    }
    return ACC_NOT_FOUND;
}*/

bool AccountManager::removeAccount(const std::string& username) {
    std::cout << "[AccountManager] Removing account: " << username
        << std::endl;
    // DONE(2026/6/7 ??:??)(2026/6/13 15:37): 删除账户（内存态缓存版本）
    ensureLoaded();
    auto it = findAccount(username);
    if (it != m_accounts.end()) {
        m_accounts.erase(it);
        saveToDisk();
        return true;
    }
    return false;
    /* 旧代码（无状态版，遍历所有 type 组）
    json j=JsonParser::parseFile(account_config_file);
    for (const AcctType &acctype : acctypels) {
        std::string accttypestr=AcctTypeToStr(acctype);
        for(size_t i=0;i<j["Account"][accttypestr].size();i++){
            if(j["Account"][accttypestr][i]["username"] == username){
                j["Account"][accttypestr].erase(i);
                JsonParser::saveToFile(j, account_config_file);
                return true;
            }
        }
    }
    //JsonParser::saveToFile(j, account_config_file);
    return false;
    */
}

/*bool AccountManager::loadFromFile(const std::string &path) {
    json j=JsonParser::parseFile(path);
}*/

// ===================== 内存态缓存辅助方法 =====================

void AccountManager::ensureLoaded() {
    if (!m_loaded) {
        loadFromDisk();
    }
}

void AccountManager::loadFromDisk() {
    m_accounts.clear();
    json j = JsonParser::parseFile(account_config_file);
    if (j.contains("Account") && j["Account"].is_object()) {
        for (auto it = j["Account"].begin(); it != j["Account"].end(); ++it) {
            if (it.value().is_array()) {
                std::vector<Account> part =
                    it.value().get<std::vector<Account>>();
                for (const Account &a : part) {
                    m_accounts.push_back(std::make_unique<Account>(a));
                }
            }
        }
    }
    m_currentAccount = j.value("current_account", "");
    m_loaded = true;
}

void AccountManager::saveToDisk() const {
    json j;
    j["Account"] = json::object();
    for (const auto &acc : m_accounts) {
        std::string key = AcctTypeToStr(acc->type);
        j["Account"][key].push_back(*acc);
    }
    j["current_account"] = m_currentAccount;
    JsonParser::saveToFile(j, account_config_file);
}

void AccountManager::persist() const {
    saveToDisk();
}

std::vector<std::unique_ptr<Account>>::iterator
AccountManager::findAccount(const std::string &username) {
    return std::find_if(
        m_accounts.begin(), m_accounts.end(),
        [&](const std::unique_ptr<Account> &p) {
            return p->username == username;
        }
    );
}
