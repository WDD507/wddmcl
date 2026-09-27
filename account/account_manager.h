#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "../app.h"
#include "../config/config_manager.h"

enum class AcctType : int {
    OffLine = 0,
    Mojang = 1,
    LittleSkin = 2,
    Other = 3,
    Microsoft = 4
}; // static_cast<AcctType>(0);
struct Account {
    std::string username="";         // 用户名
    std::string uuid="";             // UUID
    std::string accessToken="";      // 访问令牌
    AcctType type=AcctType::OffLine; // 类型 (offline/littleskin/microsoft)
    bool isValid=false;              // 是否有效
    Account(
        const std::string &_usrnm,
        const std::string &_uuid,
        const std::string &_acstkn,
        const AcctType &_type,
        bool _vld
    );
    Account(
        const std::string &_usrnm,
        const std::string &_uuid,
        const std::string &_acstkn,
        int _type,
        bool _vld
    );
    Account();
    ~Account() = default;
    friend bool operator==(const Account &a, const Account &b);
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        Account, username, uuid, accessToken, type, isValid
    )
};

extern const Account ACC_NOT_FOUND;

class AccountManager {
public:
    // 普通缓存
    std::vector<std::string> AccList={};
    std::vector<std::string> OfflineList={};
    std::vector<std::string> MojangList={};
    std::vector<std::string> LittleskinList={};
    std::vector<Account> StructureAccounts={};

    // 获取唯一实例
    static AccountManager& instance();

    AccountManager(const AccountManager&)            = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    static AcctType acctypels[5];
    static std::string AcctTypeToStr(const AcctType &type);

    int reloadAccount();

    // 添加账户
    bool addAccount(const std::string& username, const AcctType& type);
    // 添加账户（带 uuid 与 accessToken，按 type 分发到对应数组）
    bool addAccount(
        const std::string& username,
        const AcctType& type,
        const std::string& uuid,
        const std::string& accessToken
    );
    
    // 选择当前账户
    bool selectAccount(const std::string& username, const AcctType &type);
    
    // 获取当前账户
    Account getCurrentAccount();
    std::string getCurrentAccountStr() const;
    
    // 列出所有账户
    std::vector<std::string> listTypeAccounts(
        const AcctType &Type
    );
    std::vector<std::string> listOfflineAccounts();
    std::vector<std::string> listMojangAccounts();
    std::vector<std::string> listLittleskinAccounts();
    std::vector<std::string> listAccounts();

    // 根据账户名获取账户
    std::vector<Account> getStructureAccounts();
    Account getAccountFromName(const std::string &username);
    Account *getAccountFromNamePtr(const std::string &username);
    //Account &getAccountFromNameRef(const std::string &username);
    
    // 删除账户
    bool removeAccount(const std::string& username);

    // 把内存态账户信息写回磁盘（外部改完账户字段后调用）
    void persist() const;

    // 账户文件存取
    /*bool loadFromFile(const std::string &path=
#ifdef _WIN32
        ".\\accounts.json"
#else
        "./accounts.json"
#endif
    );
    bool saveToFile(const std::string &path=
#ifdef _WIN32
        ".\\accounts.json"
#else
        "./accounts.json"
#endif
    );*/

private:
    AccountManager()  = default;
    ~AccountManager() = default;

    // ===== 内存态缓存 =====
    std::vector<std::unique_ptr<Account>> m_accounts;   // 账户列表 （unique_ptr 保证元素地址在扩容时不变）
    std::string m_currentAccount;                       // 当前选中的账户名
    bool m_loaded = false;                              // 是否已从磁盘加载

    // ===== 内部辅助 =====
    void ensureLoaded();                                          // 懒加载：未加载则从磁盘读入
    void loadFromDisk();                                          // 从 accounts.json 反序列化进 m_accounts
    void saveToDisk() const;                                      // 把 m_accounts 写回 accounts.json
    std::vector<std::unique_ptr<Account>>::iterator findAccount(  // 按用户名查找，返回迭代器
        const std::string &username
    );
};

#endif // ACCOUNT_MANAGER_H
