# C++ 单例与静态风格约定

> 本文档约定本项目中"工具类 / 管理器类 / 数据类"的 static 与单例使用规范，供后续开发与重构参考。

---

## 一、分类与选型决策

在动手写一个新类前，先判断它属于哪一类：

| 类别 | 特征 | 推荐形式 | 项目示例 |
|---|---|---|---|
| 工具类 | 无状态，纯函数集合 | **全 static 函数** | `MD5Utils`、`JsonParser`、`FileSystemUtils` |
| 管理器类 | 有状态，且全局唯一 | **单例（Meyers' Singleton）** | `AccountManager`、`ConfigManager` |
| 数据类 | 有状态，多实例并存 | **普通实例类** | `Account`、`VersionInfo`、`RemoteVersionInfo` |
| 可配置服务 | 有状态但可按需配置 | **实例类**（必要时再考虑单例） | `HttpClient`（若未来支持 timeout/proxy） |

**判断口诀**：问自己"这个类在整个程序生命周期内应存在几份？"

- 0 份状态 → 全 static
- 1 份 → 单例
- N 份 → 普通实例类

---

## 二、工具类规范（全 static）

适用于无成员变量、仅提供纯函数的类。

```cpp
class JsonParser {
public:
    static json parseString(const std::string& str);
    static json parseFile(const std::string& path);
    static bool saveToFile(const json& j, const std::string& path);

    JsonParser() = delete;                              // 禁止实例化
    JsonParser(const JsonParser&) = delete;
    JsonParser& operator=(const JsonParser&) = delete;
};
```

要点：

- 构造函数 `= delete`，明确禁止实例化
- 所有方法 `static`
- 不持有任何非 `const` 成员变量
- 调用方：`JsonParser::parseString(...)`

---

## 三、管理器类规范（单例）

适用于全局唯一、需持有状态（配置路径、缓存、当前选中项等）的类。

```cpp
class AccountManager {
public:
    // 通过 instance() 获取唯一实例
    static AccountManager& instance() {
        static AccountManager inst;      // C++11 magic static，线程安全
        return inst;
    }

    AccountManager(const AccountManager&)             = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    bool addAccount(const std::string& username, const AcctType& type);
    Account getCurrentAccount() const;
    // ...其余接口保持非 static

private:
    AccountManager() = default;         // 私有构造，防止外部 new
    std::string account_config_file = "accounts.json";
    // 可选：缓存当前账户，避免每次读文件
    // Account current_;
};
```

调用方写法：

```cpp
// ✅ 正确
AccountManager::instance().addAccount("user", AcctType::Mojang);

// ❌ 禁止
static AccountManager accmgr;          // 局部 static 实例，绕过单例
accmgr.addAccount(...);

// ❌ 禁止
AccountManager acc;                    // 编译会失败（构造私有），但仍应避免此类写法
AccountManager* p = new AccountManager();
```

要点：

- 使用 Meyers' Singleton（函数内 `static` 局部变量），C++11 起线程安全
- 禁用拷贝构造与赋值
- 所有业务方法保持**非 static**，通过 `instance()` 访问
- 调用方一行链式调用：`AccountManager::instance().xxx()`

---

## 四、数据类规范（普通实例类）

适用于多实例并存的类型，如 `Account`、`VersionInfo`。

```cpp
struct Account {
    std::string username;
    std::string uuid;
    std::string accessToken;
    AcctType    type     = AcctType::OffLine;
    bool        isValid = false;
};
```

要点：

- 数据类不强制全 static，也不做单例
- 默认构造 / 拷贝语义应可用
- 由管理器或业务代码按需创建与持有

---

## 五、反模式（禁止使用）

### ❌ 1. 工具类却允许实例化

```cpp
class HttpClient {
public:
    std::string postRequest(...);      // 非 static，但其实并不需要实例
};
// 调用方被迫写：HttpClient http; http.postRequest(...);
```

**修正**：要么改全 static，要么明确"未来要支持 timeout/proxy 所以保留实例"。

### ❌ 2. 管理器类局部 static 实例

```cpp
bool AuthService::yggdrasil(...) {
    static AccountManager accmgr;       // ← 反模式
    accmgr.addAccount(...);
}
```

**问题**：若 `AccountManager` 未来加缓存成员（如 `current_`），每个调用点的局部 static 实例之间状态会不一致，难以排查。

**修正**：改为 `AccountManager::instance().addAccount(...)`。

### ❌ 3. 全局变量裸指针单例

```cpp
AccountManager* g_accmgr = new AccountManager();
```

**问题**：无线程安全保证、生命周期不受控、释放顺序未定义。

**修正**：使用 Meyers' Singleton。

### ❌ 4. 在头文件中定义 `static` 成员变量

```cpp
class Foo {
    static AccountManager s_mgr;        // 头文件中声明
};
```

**问题**：易引发多重定义或 ODR 问题。

**修正**：用 `instance()` 内部 `static` 局部变量替代。

### ⚠️ 5. 匿名 namespace 被局部变量覆盖（默认修复方案二）

匿名 namespace 里的文件级变量被局部变量同名覆盖时，无法用 `::var` 精准访问（与全局变量不同）。

```cpp
namespace {
    AccountManager& accmgr = AccountManager::instance();
}

bool AuthService::foo(const std::string& accmgr) {   // 局部 accmgr 覆盖
    // accmgr 在此处指参数，无法用 ::accmgr 精准访问文件级引用
}
```

**修正**：默认采用**方案二**（`static` 修饰符），因为 `static` 全局变量名字直接在全局作用域，可用 `::` 精准访问。

```cpp
static AccountManager& accmgr = AccountManager::instance();

bool AuthService::foo(const std::string& accmgr) {
    ::accmgr.addAccount(...);   // ✅ 可精准访问文件级引用
}
```

> 注：仅在实际出现命名冲突时才需修复；未冲突时维持匿名 namespace 原样，不为统一而统一。

---

## 六、迁移策略

不要求一次性重构。原则：**"碰到哪个文件改哪个文件，且新增代码必须遵守本约定"**。

优先级建议：

1. 🔴 高：`AccountManager` 改单例（当前已有局部 static 调用，风险最高）
2. 🟡 中：`HttpClient` 决策——保留实例还是改全 static
3. 🟢 低：纯工具类（`MD5Utils` 等）已符合规范，无需改动

---

## 七、命名约定

| 角色 | 命名风格 | 示例 |
|---|---|---|
| 单例获取方法 | `instance()` | `AccountManager::instance()` |
| 管理器类名 | `XxxManager` | `ConfigManager`、`VersionManager` |
| 工具类名 | `XxxUtils` / `XxxParser` | `MD5Utils`、`JsonParser` |
| 数据类名 | 普通名词 | `Account`、`VersionInfo` |

---

## 八、检查清单（Code Review 用）

新增或修改一个类时，自检以下问题：

- [ ] 该类是否需要持有状态？
- [ ] 如果不需要状态，是否已将构造函数 `= delete` 且方法全 static？
- [ ] 如果需要状态且全局唯一，是否已用 `instance()` 单例？
- [ ] 单例类是否已 `delete` 拷贝构造与赋值？
- [ ] 调用方是否避免了 `static XxxManager mgr;` 这类局部 static 实例？
- [ ] 头文件中是否避免了定义 `static` 成员变量？

---

*文档版本：v1.1    日期：2026-08-23*
*变更：新增反模式第 5 条（匿名 namespace 被覆盖时的默认修复方案）*
*适用项目：WDD's Minecraft Launcher (wddmcl)*
