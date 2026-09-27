# Minecraft Launcher - 控制台版本

这是一个正在开发中的Minecraft启动器，目前为控制台版本（无GUI）。

## 📁 项目结构

```
wddmcl/
├── main.cpp                    # 程序入口
├── launcher.h/cpp              # 启动器主类
├── build.bat                   # 编译脚本（批处理）
├── compile_mingw.ps1          # 编译脚本（PowerShell）
│
├── minecraft/                  # Minecraft相关模块
│   ├── version_manager.h/cpp   # 版本管理
│   └── game_launcher.h/cpp     # 游戏启动核心
│
├── account/                    # 账户管理模块
│   ├── account_manager.h/cpp   # 账户管理
│   └── auth_service.h/cpp      # 认证服务
│
├── java/                       # Java运行时模块
│   ├── java_detector.h/cpp     # Java检测
│   └── java_runtime.h/cpp      # Java运行时管理
│
├── mod/                        # Mod管理模块
│   ├── mod_manager.h/cpp       # Mod管理
│   └── forge_installer.h/cpp   # Forge安装
│
├── utils/                      # 工具模块
│   ├── file_utils.h/cpp        # 文件工具
│   ├── http_client.h/cpp       # HTTP客户端
│   └── json_parser.h/cpp       # JSON解析
│
└── config/                     # 配置管理模块
    └── config_manager.h/cpp    # 配置管理
```

## 🔧 编译方法

### 方法一：使用批处理脚本（推荐）

直接双击运行 `build.bat` 或在命令行中执行：

```bash
.\build.bat
```

### 方法二：使用PowerShell脚本

如果允许PowerShell执行脚本：

```powershell
.\compile_mingw.ps1
```

### 方法三：手动编译

```bash
# 设置环境变量
set "PATH=C:\Program Files (x86)\Dev-Cpp\MinGW64\bin;%PATH%"

# 编译所有源文件
g++.exe -I. -I./minecraft -I./account -I./java -I./mod -I./utils -I./config ^
    -std=c++17 ^
    main.cpp launcher.cpp ^
    minecraft/*.cpp ^
    account/*.cpp ^
    java/*.cpp ^
    mod/*.cpp ^
    utils/*.cpp ^
    config/*.cpp ^
    -o launcher.exe
```

## 🚀 运行程序

编译成功后，运行：

```bash
.\launcher.exe
```

## 📝 当前状态

- ✅ 项目框架已搭建
- ✅ 所有模块的头文件和实现文件已创建
- ✅ 基础编译脚本已完成
- ⏳ 各模块功能待实现（标记为TODO）

## 🎯 下一步计划

1. **实现文件工具** - 完善 `file_utils.cpp` 中的文件操作功能
2. **实现配置管理** - 完成配置的读写功能
3. **实现Java检测** - 自动检测系统中的Java安装
4. **实现版本管理** - 从Mojang API获取版本列表
5. **实现游戏启动** - 构建完整的启动命令并启动游戏
6. **实现账户系统** - 支持离线和微软账户认证
7. **实现Mod管理** - Mod的安装、卸载和管理
8. **添加HTTP客户端** - 集成libcurl或其他HTTP库
9. **添加JSON解析** - 集成nlohmann/json库

## 💡 开发提示

- 所有源文件都已创建，但大部分功能标记为TODO
- 可以逐个模块实现功能
- 建议使用MinGW64编译器（已配置在 `C:\Program Files (x86)\Dev-Cpp\MinGW64`）
- 编译时会自动包含所有子目录的头文件路径

## 📦 需要添加的第三方库

后续开发需要以下库：
- **nlohmann/json** - JSON解析（header-only）
- **libcurl** 或 **cpr** - HTTP请求
- **minizip** - ZIP文件解压

## 📄 许可证

本项目仅供学习和个人使用。
