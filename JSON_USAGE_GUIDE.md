# JSON 库使用教程

## ✅ 已完成

nlohmann/json 库已经成功集成到项目中！

- 📁 位置：`third_party/nlohmann/json.hpp`
- 📄 大小：约 900KB（单个头文件）
- ✨ 特性：header-only，无需编译，直接使用

---

## 🚀 快速开始

### 1. 测试 JSON 库

运行测试程序：

```bash
# 编译测试程序
g++.exe -I. -I./third_party -std=c++17 test_json_usage.cpp -o test_json.exe

# 运行
.\test_json.exe
```

或者使用批处理脚本（需要先修改以支持单文件编译）。

---

## 📖 基本用法

### 引入头文件

```cpp
#include "utils/json_parser.h"
// 或者直接使用
#include "third_party/nlohmann/json.hpp"

using json = nlohmann::json;
```

### 创建 JSON 对象

```cpp
// 方法1：直接创建
json j;
j["name"] = "Minecraft";
j["version"] = "1.20.1";
j["mods_enabled"] = true;
j["memory_mb"] = 4096;

// 方法2：从初始化列表
json j2 = {
    {"name", "Minecraft"},
    {"version", "1.20.1"},
    {"mods", {"optifine", "forge"}}
};

// 方法3：嵌套对象
json config;
config["graphics"]["resolution"]["width"] = 1920;
config["graphics"]["resolution"]["height"] = 1080;
config["graphics"]["render_distance"] = 12;
```

### 访问 JSON 数据

```cpp
// 获取字符串
std::string name = j["name"].get<std::string>();

// 获取整数
int memory = j["memory_mb"].get<int>();

// 获取布尔值
bool enabled = j["mods_enabled"].get<bool>();

// 获取数组
for (const auto& mod : j["mods"]) {
    std::cout << mod.get<std::string>() << std::endl;
}

// 访问嵌套对象
int width = config["graphics"]["resolution"]["width"].get<int>();
```

### 检查键是否存在

```cpp
if (j.contains("name")) {
    std::cout << "Name exists: " << j["name"] << std::endl;
}

// 或者提供默认值
std::string nickname = j.value("nickname", "Player");
```

### 遍历 JSON 对象

```cpp
// 遍历所有键值对
for (auto& [key, value] : j.items()) {
    std::cout << key << ": " << value << std::endl;
}

// 遍历数组
for (size_t i = 0; i < j["mods"].size(); i++) {
    std::cout << j["mods"][i] << std::endl;
}
```

### 保存和加载 JSON 文件

```cpp
#include "utils/json_parser.h"

// 保存到文件
json config;
config["java_path"] = "C:/Java/jdk-17/bin/java.exe";
config["memory"] = 4096;

JsonParser::saveToFile(config, "config.json");

// 从文件加载
try {
    json loaded = JsonParser::parseFile("config.json");
    std::cout << "Java path: " << loaded["java_path"].get<std::string>() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### 解析 JSON 字符串

```cpp
std::string jsonString = R"({
    "name": "Minecraft",
    "version": "1.20.1"
})";

json parsed = JsonParser::parseString(jsonString);
std::cout << parsed["name"] << std::endl;
```

---

## 🎮 在 Minecraft 启动器中的应用

### 1. 存储启动器配置

```cpp
// 创建配置
json launcherConfig;
launcherConfig["java_path"] = "C:/Program Files/Java/jdk-17/bin/java.exe";
launcherConfig["memory_mb"] = 4096;
launcherConfig["fullscreen"] = false;
launcherConfig["recent_versions"] = {"1.20.1", "1.19.4", "1.18.2"};

// 保存配置
JsonParser::saveToFile(launcherConfig, "launcher_config.json");
```

### 2. 解析 Minecraft 版本信息

Mojang 的版本清单是 JSON 格式：

```cpp
// 从 API 获取版本列表（需要 HTTP 客户端）
std::string versionsJsonStr = httpClient.getRequest(
    "https://launchermeta.mojang.com/mc/game/version_manifest.json"
);

// 解析 JSON
json versions = JsonParser::parseString(versionsJsonStr);

// 获取最新版本
std::string latestRelease = versions["latest"]["release"].get<std::string>();
std::cout << "Latest release: " << latestRelease << std::endl;

// 遍历所有版本
for (const auto& version : versions["versions"]) {
    std::string id = version["id"].get<std::string>();
    std::string type = version["type"].get<std::string>();
    std::cout << id << " (" << type << ")" << std::endl;
}
```

### 3. 解析版本详情

每个版本的 `version.json` 文件：

```cpp
// 加载版本详情
json versionInfo = JsonParser::parseFile(".minecraft/versions/1.20.1/1.20.1.json");

// 获取主类
std::string mainClass = versionInfo["mainClass"].get<std::string>();

// 获取依赖库
auto libraries = JsonParser::extractLibraries(versionInfo);
for (const auto& lib : libraries) {
    std::cout << "Library: " << lib.name << std::endl;
    std::cout << "Path: " << lib.path << std::endl;
}

// 获取资产索引
std::string assetIndex = versionInfo["assetIndex"]["id"].get<std::string>();
```

### 4. 管理 Mod 列表

```cpp
// 创建 Mod 列表
json modList;
modList["version"] = "1.20.1";
modList["loader"] = "forge";

json mod1;
mod1["name"] = "OptiFine";
mod1["file"] = "OptiFine_1.20.1_HD_U.jar";
mod1["enabled"] = true;

json mod2;
mod2["name"] = "JEI";
mod2["file"] = "jei-1.20.1-forge.jar";
mod2["enabled"] = false;

modList["mods"] = {mod1, mod2};

// 保存
JsonParser::saveToFile(modList, "mods_list.json");
```

---

## 💡 常用技巧

### 1. 美化输出

```cpp
// 紧凑格式
std::cout << j.dump() << std::endl;

// 格式化（4空格缩进）
std::cout << j.dump(4) << std::endl;

// 自定义缩进
std::cout << j.dump('\t') << std::endl;  // 使用制表符
```

### 2. 类型转换

```cpp
// 自动类型推断
int num = j["number"];  // 如果值是整数

// 显式转换（更安全）
int num = j["number"].get<int>();

// 带默认值
int num = j.value("number", 0);  // 如果不存在则返回0
```

### 3. 错误处理

```cpp
try {
    json j = JsonParser::parseFile("config.json");
    std::string value = j["key"].get<std::string>();
} catch (const json::exception& e) {
    std::cerr << "JSON error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### 4. 合并 JSON 对象

```cpp
json defaults = {
    {"memory", 2048},
    {"fullscreen", false}
};

json userConfig = {
    {"memory", 4096}
};

// 合并（userConfig 覆盖 defaults）
defaults.update(userConfig);
```

### 5. 删除 JSON 中的项（新增）

```cpp
json config;
config["name"] = "Minecraft";
config["version"] = "1.20.1";
config["debug_mode"] = true;
config["mods"] = {"optifine", "forge", "jei"};

// 方法1：使用 erase() 删除单个键
config.erase("debug_mode");
// 结果: {"name": "Minecraft", "version": "1.20.1", "mods": [...]}

// 方法2：从数组中删除元素
config["mods"].erase(0);  // 删除第一个元素（索引0）
// 结果: mods = ["forge", "jei"]

// 方法3：根据值删除数组元素
auto& mods = config["mods"];
for (auto it = mods.begin(); it != mods.end(); ) {
    if (*it == "jei") {
        it = mods.erase(it);  // 返回下一个迭代器
    } else {
        ++it;
    }
}
// 结果: mods = ["forge"]

// 方法4：清空整个对象或数组
config["mods"].clear();  // 清空数组
config.clear();          // 清空整个对象

// 方法5：条件删除
if (config.contains("old_setting")) {
    config.erase("old_setting");
}
```

### 6. 修改和更新 JSON（新增）

```cpp
json config;
config["settings"]["resolution"] = 1920;
config["settings"]["fullscreen"] = false;

// 修改现有值
config["settings"]["resolution"] = 2560;  // 直接赋值覆盖

// 使用 update() 批量更新
json newSettings = {
    {"resolution", 3840},
    {"fullscreen", true},
    {"render_distance", 16}
};
config["settings"].update(newSettings);

// 部分更新（只更新存在的键）
json partialUpdate = {{"resolution", 1920}};
for (auto& [key, value] : partialUpdate.items()) {
    if (config["settings"].contains(key)) {
        config["settings"][key] = value;
    }
}
```

### 7. JSON 类型检查和转换（新增）

```cpp
json j;
j["number"] = 42;
j["text"] = "hello";
j["array"] = {1, 2, 3};
j["object"] = {{"key", "value"}};
j["null_value"] = nullptr;
j["boolean"] = true;

// 检查类型
if (j["number"].is_number()) {
    std::cout << "是数字" << std::endl;
}

if (j["text"].is_string()) {
    std::cout << "是字符串" << std::endl;
}

if (j["array"].is_array()) {
    std::cout << "是数组" << std::endl;
}

if (j["object"].is_object()) {
    std::cout << "是对象" << std::endl;
}

if (j["null_value"].is_null()) {
    std::cout << "是null" << std::endl;
}

if (j["boolean"].is_boolean()) {
    std::cout << "是布尔值" << std::endl;
}

// 安全地获取值（带默认值）
int num = j.value("missing_key", 0);  // 如果不存在返回0
std::string text = j.value("missing_key", "default");  // 返回"default"

// 尝试转换类型
try {
    int num = j["text"].get<int>();  // 会抛出异常
} catch (const json::type_error& e) {
    std::cerr << "类型错误: " << e.what() << std::endl;
}
```

### 8. 处理嵌套结构（新增）

```cpp
// 创建复杂的嵌套结构
json minecraft;
minecraft["launcher"]["version"] = "1.0.0";
minecraft["launcher"]["config"]["java"]["path"] = "C:/Java/jdk-17/bin/java.exe";
minecraft["launcher"]["config"]["java"]["memory"] = 4096;
minecraft["launcher"]["config"]["game"]["version"] = "1.20.1";
minecraft["launcher"]["config"]["game"]["mods"] = {
    {"name", "OptiFine", "enabled", true},
    {"name", "JEI", "enabled", false}
};

// 访问深层嵌套
std::string javaPath = minecraft["launcher"]["config"]["java"]["path"];

// 安全访问（检查每一层是否存在）
if (minecraft.contains("launcher") &&
    minecraft["launcher"].contains("config") &&
    minecraft["launcher"]["config"].contains("java")) {
    std::string path = minecraft["launcher"]["config"]["java"]["path"];
}

// 使用指针简化访问
auto* javaConfig = minecraft.find("launcher");
if (javaConfig != minecraft.end()) {
    auto* config = javaConfig->find("config");
    if (config != javaConfig->end()) {
        // 继续访问...
    }
}
```

---

## 📚 更多资源

- **官方文档**: https://json.nlohmann.me/
- **GitHub**: https://github.com/nlohmann/json
- **示例代码**: 查看 `test_json_usage.cpp`

---

## ❓ 常见问题

### Q: 如何处理中文？
A: nlohmann/json 完全支持 UTF-8，可以直接使用中文字符串。

### Q: JSON 文件太大怎么办？
A: 可以使用流式解析或只读取需要的部分。

### Q: 如何验证 JSON 格式？
A: 使用在线工具如 https://jsonlint.com/ 或在代码中使用 try-catch。

### Q: 性能如何？
A: 对于启动器配置等小文件，性能完全足够。如果需要处理大文件，可以考虑其他库。

---

## 🎯 下一步

现在 JSON 库已经可以使用了，接下来可以：

1. ✅ 实现配置管理系统
2. ✅ 解析 Mojang API 返回的版本列表
3. ✅ 存储和管理 Mod 列表
4. ⏳ 集成 HTTP 客户端来获取在线数据

祝你开发顺利！🚀
