# 第三方库安装指南

## 📦 需要安装的库

1. **nlohmann/json** - JSON解析库（header-only）
2. **cpr** - C++ HTTP请求库（基于libcurl）

---

## 🔧 方法一：手动下载（推荐初学者）

### 1. 安装 nlohmann/json

#### 步骤：
1. 访问 releases 页面：https://github.com/nlohmann/json/releases
2. 找到最新的版本（如 v3.11.3）
3. 下载 `single_include.zip` 或 `json.hpp` 文件
4. 解压后，将 `json.hpp` 文件复制到项目的 `third_party/nlohmann/` 目录

最终结构应该是：
```
third_party/
└── nlohmann/
    └── json.hpp
```

### 2. 安装 cpr（C++ Requests）

cpr有多种安装方式，选择其中一种：

#### 选项A：使用预编译库（最简单）
1. 访问：https://github.com/libcpr/cpr/releases
2. 下载最新的 Windows 版本（如果有提供）
3. 解压到 `third_party/cpr/`

#### 选项B：使用 vcpkg（推荐）
如果你已经安装了 vcpkg：
```bash
vcpkg install cpr:x64-windows
```

#### 选项C：从源码编译（较复杂）
1. 克隆仓库：
```bash
git clone https://github.com/libcpr/cpr.git
cd cpr
mkdir build && cd build
cmake .. -DCPR_BUILD_TESTS=OFF
cmake --build . --config Release
```

2. 将编译好的库文件复制到 `third_party/cpr/`

---

## 🔧 方法二：使用包管理器（推荐高级用户）

### 使用 vcpkg

1. **安装 vcpkg**（如果还没有）：
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

2. **安装所需的库**：
```bash
.\vcpkg install nlohmann-json:x64-windows
.\vcpkg install cpr:x64-windows
```

3. **集成到 Visual Studio**（可选）：
```bash
.\vcpkg integrate install
```

4. **修改编译脚本**以使用 vcpkg 的路径

---

## ✅ 验证安装

### 测试 nlohmann/json

创建一个测试文件 `test_json.cpp`：

```cpp
#include <iostream>
#include "third_party/nlohmann/json.hpp"

using json = nlohmann::json;

int main() {
    // 创建JSON对象
    json j;
    j["name"] = "Minecraft";
    j["version"] = "1.20.1";
    j["mods"] = {"optifine", "forge"};
    
    // 输出JSON
    std::cout << j.dump(4) << std::endl;
    
    return 0;
}
```

编译并运行：
```bash
g++.exe -I. -std=c++17 test_json.cpp -o test_json.exe
.\test_json.exe
```

应该输出：
```json
{
    "mods": [
        "optifine",
        "forge"
    ],
    "name": "Minecraft",
    "version": "1.20.1"
}
```

### 测试 cpr

创建一个测试文件 `test_http.cpp`：

```cpp
#include <iostream>
#include "third_party/cpr/cpr.h"

int main() {
    // 发送GET请求
    auto response = cpr::Get(cpr::Url{"https://api.github.com"});
    
    std::cout << "Status Code: " << response.status_code << std::endl;
    std::cout << "Response: " << response.text.substr(0, 200) << std::endl;
    
    return 0;
}
```

---

## 📝 更新项目代码

安装完成后，需要更新以下文件：

### 1. 更新 `utils/json_parser.h`

```cpp
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include "third_party/nlohmann/json.hpp"

using json = nlohmann::json;

struct Library {
    std::string name;
    std::string version;
    std::string path;
};

namespace JsonParser {
    // 解析JSON文件
    json parseFile(const std::string& filePath);
    
    // 解析JSON字符串
    json parseString(const std::string& jsonString);
    
    // 保存JSON到文件
    bool saveToFile(const json& data, const std::string& filePath);
    
    // 从版本JSON提取信息
    // VersionInfo extractVersionInfo(const json& versionJson);
    
    // 从库JSON提取依赖
    std::vector<Library> extractLibraries(const json& versionJson);
}

#endif // JSON_PARSER_H
```

### 2. 更新 `utils/json_parser.cpp`

```cpp
#include "json_parser.h"
#include <iostream>
#include <fstream>

namespace JsonParser {
    json parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }
        
        json j;
        file >> j;
        return j;
    }
    
    json parseString(const std::string& jsonString) {
        return json::parse(jsonString);
    }
    
    bool saveToFile(const json& data, const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << data.dump(4);  // 4空格缩进
        return true;
    }
    
    std::vector<Library> extractLibraries(const json& versionJson) {
        std::vector<Library> libraries;
        
        if (versionJson.contains("libraries")) {
            for (const auto& lib : versionJson["libraries"]) {
                Library library;
                if (lib.contains("name")) {
                    library.name = lib["name"].get<std::string>();
                }
                if (lib.contains("version")) {
                    library.version = lib["version"].get<std::string>();
                }
                libraries.push_back(library);
            }
        }
        
        return libraries;
    }
}
```

### 3. 更新 `utils/http_client.h`

```cpp
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <functional>
#include "third_party/cpr/cpr.h"

class HttpClient {
public:
    // 下载文件
    bool downloadFile(const std::string& url, const std::string& savePath);
    
    // 下载文件（带进度回调）
    bool downloadFileWithProgress(
        const std::string& url,
        const std::string& savePath,
        std::function<void(double)> progressCallback
    );
    
    // GET请求
    std::string getRequest(const std::string& url);
    
    // POST请求
    std::string postRequest(const std::string& url, const std::string& data);
    
    // 设置超时
    void setTimeout(int seconds);
    
private:
    int timeoutSeconds = 30;
};

#endif // HTTP_CLIENT_H
```

### 4. 更新 `utils/http_client.cpp`

```cpp
#include "http_client.h"
#include <iostream>
#include <fstream>

bool HttpClient::downloadFile(const std::string& url, const std::string& savePath) {
    std::cout << "[HttpClient] Downloading: " << url << std::endl;
    
    auto response = cpr::Download(
        cpr::Url{url},
        cpr::WriteFile{cpr::FileName{savePath}},
        cpr::Timeout{timeoutSeconds * 1000}
    );
    
    return response.status_code == 200;
}

bool HttpClient::downloadFileWithProgress(
    const std::string& url,
    const std::string& savePath,
    std::function<void(double)> progressCallback
) {
    std::cout << "[HttpClient] Downloading with progress: " << url << std::endl;
    
    // cpr的进度回调功能
    auto response = cpr::Download(
        cpr::Url{url},
        cpr::WriteFile{cpr::FileName{savePath}},
        cpr::ProgressCallback([&](cpr::cpr_off_t downloadTotal, 
                                   cpr::cpr_off_t downloadNow,
                                   cpr::cpr_off_t uploadTotal,
                                   cpr::cpr_off_t uploadNow) -> bool {
            if (downloadTotal > 0) {
                double progress = static_cast<double>(downloadNow) / downloadTotal;
                progressCallback(progress);
            }
            return true;  // 继续下载
        }),
        cpr::Timeout{timeoutSeconds * 1000}
    );
    
    return response.status_code == 200;
}

std::string HttpClient::getRequest(const std::string& url) {
    std::cout << "[HttpClient] GET: " << url << std::endl;
    
    auto response = cpr::Get(
        cpr::Url{url},
        cpr::Timeout{timeoutSeconds * 1000}
    );
    
    if (response.status_code == 200) {
        return response.text;
    } else {
        std::cerr << "[HttpClient] Error: " << response.status_code << std::endl;
        return "";
    }
}

std::string HttpClient::postRequest(const std::string& url, const std::string& data) {
    std::cout << "[HttpClient] POST: " << url << std::endl;
    
    auto response = cpr::Post(
        cpr::Url{url},
        cpr::Body{data},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Timeout{timeoutSeconds * 1000}
    );
    
    if (response.status_code == 200) {
        return response.text;
    } else {
        std::cerr << "[HttpClient] Error: " << response.status_code << std::endl;
        return "";
    }
}

void HttpClient::setTimeout(int seconds) {
    timeoutSeconds = seconds;
}
```

---

## 🔨 更新编译脚本

### 更新 `build.bat`

在编译命令中添加第三方库的包含路径：

```batch
REM 编译命令
g++.exe ^
    -I. ^
    -I./minecraft ^
    -I./account ^
    -I./java ^
    -I./mod ^
    -I./utils ^
    -I./config ^
    -I./third_party ^
    -std=c++17 ^
    %SOURCES% ^
    -o launcher.exe ^
    -L./third_party/cpr/lib ^
    -lcpr -llibcurl
```

### 如果使用 vcpkg

```batch
REM 设置vcpkg路径
set "VCPKG_ROOT=C:\path\to\vcpkg"
set "INCLUDE_PATH=%VCPKG_ROOT%\installed\x64-windows\include"
set "LIB_PATH=%VCPKG_ROOT%\installed\x64-windows\lib"

g++.exe ^
    -I. ^
    -I./minecraft ^
    -I./account ^
    -I./java ^
    -I./mod ^
    -I./utils ^
    -I./config ^
    -I%INCLUDE_PATH% ^
    -std=c++17 ^
    %SOURCES% ^
    -o launcher.exe ^
    -L%LIB_PATH% ^
    -lcpr -llibcurl -lws2_32 -lwldap32 -lnormaliz
```

---

## 💡 使用示例

### JSON使用示例

```cpp
#include "utils/json_parser.h"

// 解析Minecraft版本列表
json versionsJson = JsonParser::parseFile("versions.json");

// 访问数据
std::string latestRelease = versionsJson["latest"]["release"];
std::cout << "Latest release: " << latestRelease << std::endl;

// 遍历版本列表
for (const auto& version : versionsJson["versions"]) {
    std::cout << "Version: " << version["id"].get<std::string>() << std::endl;
}

// 创建JSON
json newConfig;
newConfig["java_path"] = "C:/Program Files/Java/jdk-17/bin/java.exe";
newConfig["memory"] = 4096;
newConfig["fullscreen"] = false;

// 保存配置
JsonParser::saveToFile(newConfig, "config.json");
```

### HTTP使用示例

```cpp
#include "utils/http_client.h"

HttpClient http;

// 获取Minecraft版本列表
std::string versionsJson = http.getRequest(
    "https://launchermeta.mojang.com/mc/game/version_manifest.json"
);

// 下载游戏文件
http.downloadFile(
    "https://launcher.mojang.com/v1/objects/xxx/server.jar",
    "minecraft_server.jar"
);

// 带进度的下载
http.downloadFileWithProgress(
    "https://example.com/large_file.zip",
    "large_file.zip",
    [](double progress) {
        std::cout << "Progress: " << (progress * 100) << "%" << std::endl;
    }
);
```

---

## ❓ 常见问题

### Q: 编译时找不到头文件？
A: 确保 `-I` 参数包含了 `third_party` 目录

### Q: 链接错误？
A: 确保添加了 `-L` 和 `-l` 参数指定库路径和库名

### Q: cpr编译失败？
A: cpr依赖libcurl，确保libcurl也已正确安装

### Q: 不想用cpr，有其他选择吗？
A: 可以使用：
- libcurl（C语言，更底层）
- Boost.Beast（需要Boost库）
- Qt Network（如果使用Qt）

---

## 📚 参考资料

- nlohmann/json 文档：https://json.nlohmann.me/
- cpr 文档：https://libcpr.org/
- libcurl 文档：https://curl.se/libcurl/
