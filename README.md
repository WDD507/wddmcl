# WDD's Minecraft Launcher (wddmcl)

一个用 C++17 编写的 Minecraft 启动器，支持离线登录和外置登录（Mojang / LittleSkin），自动下载游戏版本、资源文件和原生库。

> 状态：Beta v0.0.5.1

***

## ✨ 功能特性

- **账户管理**
  - 离线账户（用户名 + MD5 令牌）
  - Mojang / LittleSkin 外置登录（Yggdrasil 协议）
  - 令牌有效性校验与自动刷新
- **版本管理**
  - 从 Mojang 服务器获取版本列表
  - 自动下载 `client.jar`、依赖库（libraries）、原生库（natives）、资源文件（assets）
  - 完整的 library `rules` 规则求值（按 OS / 特性过滤）
- **游戏启动**
  - 解析 `version.json` 构建启动命令
  - 自动拼接 classpath（基于 libraries 列表，而非通配符）
  - 解压 natives 到 `versions/<版本>/natives` 并设置 `java.library.path`
- **工具模块**
  - HTTP 客户端（基于 cpr/libcurl）
  - ZIP 解压（基于 minizip，支持排除指定目录）
  - MD5 计算、JSON 解析、跨平台进程启动

***

## 📁 项目结构

```
wddmcl/
├── main.cpp                    # 程序入口
├── launcher.h/cpp              # 启动器主类（持有 ConfigManager 和 VersionManager）
├── app.h/cpp                   # 应用框架与平台检测
├── build.bat                   # 编译脚本
│
├── minecraft/                  # Minecraft 相关
│   ├── version_manager.h/cpp   # 版本管理与下载
│   └── game_launcher.h/cpp     # 游戏启动命令构建
│
├── account/                    # 账户与认证
│   ├── account_manager.h/cpp   # 账户管理（内存缓存 + 持久化）
│   └── auth_service.h/cpp      # 认证服务（离线 / Yggdrasil）
│
├── java/                       # Java 运行时
│   ├── java_detector.h/cpp     # Java 检测
│   └── java_runtime.h/cpp      # Java 运行时管理
│
├── mod/                        # Mod 管理（开发中）
│   ├── mod_manager.h/cpp       # Mod 管理
│   └── forge_installer.h/cpp   # Forge 安装
│
├── utils/                      # 工具模块
│   ├── http_client.h/cpp       # HTTP 客户端
│   ├── zip_utils.h/cpp         # ZIP 解压封装（minizip）
│   ├── file_system_utils.h/cpp # 文件系统工具
│   ├── md5_utils.h/cpp         # MD5 计算
│   └── json_parser.h/cpp       # JSON 解析封装
│
├── config/                     # 配置管理
│   ├── config_manager.h/cpp    # 应用配置
│   └── game_config.h/cpp       # 游戏目录配置（game_config.json）
│
└── third_party/                # 第三方库
    ├── libcpr/                 # cpr (HTTP)
    ├── nlohmann/               # JSON 解析
    ├── awtk/                   # AWTK (GUI)
    └── zlib/zlib-1.3.2/        # zlib + minizip (ZIP 解压)
```

***

## 🔧 编译

### 环境要求

- **编译器**：MinGW64 g++（C++17），或 MSVC
- **第三方库**：已包含在 `third_party/` 中，无需额外安装

### 使用脚本编译（推荐）

```bash
.\build.bat
```

### 手动编译

```bash
g++ -I. -I./minecraft -I./account -I./java -I./mod -I./utils -I./config -I./third_party -I./third_party/libcpr/include -I./third_party/zlib/zlib-1.3.2 -I./third_party/zlib/zlib-1.3.2/contrib/minizip -std=c++17 ^
    main.cpp launcher.cpp app.cpp ^
    minecraft/*.cpp ^
    account/*.cpp ^
    java/*.cpp ^
    mod/*.cpp ^
    utils/*.cpp ^
    config/*.cpp ^
    third_party/zlib/zlib-1.3.2/contrib/minizip/unzip.c ^
    third_party/zlib/zlib-1.3.2/contrib/minizip/ioapi.c ^
    -Lthird_party/libcpr/lib -lcpr -lcurl -lm ^
    -o launcher.exe
```

编译时需要链接 zlib（minizip 解压依赖），zlib 源码位于 `third_party/zlib/zlib-1.3.2/`。

***

## 🚀 运行

```bash
.\launcher.exe
```

首次运行需要：

1. 配置 Java 路径（自动检测或手动设置）
2. 下载游戏版本
3. 添加账户（离线或外置登录）
4. 启动游戏

***

## 📝 配置文件

### `game_config.json` — 游戏目录配置

```json
{
    "directory": {
        "default": {
            "${UserName}": "C:\\Users\\${UserName}\\AppData\\Roaming\\.minecraft",
            "Everyone": "${AppDirPath}\\.minecraft"
        },
        "lists": [],
        "current": "Everyone"
    }
}
```

- `${UserName}`：当前系统用户名
- `${AppDirPath}`：启动器所在目录
- `current` 字段决定当前使用的目录配置

### `accounts.json` — 账户数据

由程序自动管理，存储在启动器目录下。

***

## 🧩 技术栈

| 组件                   | 说明          |
| -------------------- | ----------- |
| C++17                | 开发语言        |
| nlohmann/json        | JSON 解析     |
| cpr + libcurl        | HTTP 请求     |
| minizip (zlib-1.3.2) | ZIP 解压      |
| AWTK                 | GUI 框架（开发中） |

***

## 📋 开发计划

- [x] 离线账户认证
- [x] Mojang / LittleSkin 外置登录
- [x] 令牌有效性校验与刷新
- [x] 版本列表获取与下载
- [x] libraries / natives / assets 下载
- [x] 游戏启动命令构建
- [ ] 微软账户 OAuth 登录
- [ ] Forge / Mod 管理
- [ ] GUI 界面（AWTK）
- [ ] 自动下载 Java 运行时
- [ ] 跨平台支持（Linux / 鸿蒙）

***

## 📄 许可证

**MIT**
<!--本项目仅供学习和个人使用。-->
