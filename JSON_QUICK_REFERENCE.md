# JSON 库快速参考卡

## 🚀 快速开始

### 引入头文件
```cpp
#include "utils/json_parser.h"
// 或
#include "third_party/nlohmann/json.hpp"
using json = nlohmann::json;
```

---

## 📝 常用操作速查

### 创建 JSON
```cpp
json j;
j["key"] = "value";
j["number"] = 42;
j["array"] = {1, 2, 3};
j["nested"]["key"] = "value";
```

### 读取数据
```cpp
string s = j["key"].get<string>();
int n = j["number"].get<int>();
bool b = j["flag"].get<bool>();
```

### 保存/加载
```cpp
JsonParser::saveToFile(j, "file.json");
json loaded = JsonParser::parseFile("file.json");
```

### 解析字符串
```cpp
json j = JsonParser::parseString("{\"key\": \"value\"}");
```

---

## 🎮 Minecraft 启动器实用示例

### 配置管理
```cpp
json config;
config["java_path"] = "C:/Java/jdk-17/bin/java.exe";
config["memory_mb"] = 4096;
JsonParser::saveToFile(config, "config.json");
```

### 账户列表
```cpp
json accounts;
accounts[0]["username"] = "Player";
accounts[0]["type"] = "offline";
accounts[1]["username"] = "Admin";
accounts[1]["type"] = "microsoft";
```

### Mod 管理
```cpp
json mod;
mod["name"] = "OptiFine";
mod["enabled"] = true;
mod["file"] = "optifine.jar";
```

---

## 🔍 检查与遍历

### 检查键存在
```cpp
if (j.contains("key")) { /* exists */ }
```

### 带默认值
```cpp
string val = j.value("key", "default");
```

### 遍历数组
```cpp
for (const auto& item : j["array"]) {
    cout << item.get<string>() << endl;
}
```

### 遍历对象
```cpp
for (auto& [key, value] : j.items()) {
    cout << key << ": " << value << endl;
}
```

---

## 💾 文件操作

### 保存（美化格式）
```cpp
JsonParser::saveToFile(j, "config.json");  // 4空格缩进
```

### 加载（错误处理）
```cpp
try {
    json j = JsonParser::parseFile("config.json");
} catch (const exception& e) {
    cerr << "Error: " << e.what() << endl;
}
```

---

## 🎯 常见模式

### 合并配置
```cpp
json defaults = {{"memory", 2048}};
json user = {{"memory", 4096}};
defaults.update(user);  // user 覆盖 defaults
```

### 添加数组元素
```cpp
j["versions"].push_back("1.20.1");
j["versions"].push_back("1.19.4");
```

### 删除键
```cpp
j.erase("unwanted_key");
```

### 获取大小
```cpp
int size = j["array"].size();
```

---

## ⚠️ 注意事项

1. **类型安全**: 使用 `.get<type>()` 而非直接赋值
2. **错误处理**: 文件操作要包裹在 try-catch 中
3. **UTF-8**: 完全支持中文字符串
4. **性能**: 小文件性能优秀，大文件考虑流式处理

---

## 📚 更多信息

- 完整教程: `JSON_USAGE_GUIDE.md`
- 测试代码: `test_json_usage.cpp`
- 实际示例: `example_launcher_config.cpp`
- 官方文档: https://json.nlohmann.me/

---

*打印此页作为快速参考！*
