# C++ 编译器使用说明

## 已配置的编译器

你的系统中已经安装了两个C++编译器：

### 1. MSVC (Microsoft Visual C++)
- **位置**: `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207`
- **编译器**: cl.exe
- **特点**: 微软官方编译器，与Windows集成度高

### 2. MinGW64 (GCC for Windows)
- **位置**: `C:\Program Files (x86)\Dev-Cpp\MinGW64`
- **编译器**: g++.exe
- **特点**: GNU编译器集合，跨平台兼容性好

## 使用方法

### 方法一：使用批处理脚本（推荐）

#### 使用 MSVC 编译：
```bash
.\build_msvc.bat main.cpp
```

#### 使用 MinGW64 编译：
```bash
.\build_mingw.bat main.cpp
```

脚本会自动：
- 设置环境变量
- 编译程序
- 询问是否运行程序

### 方法二：手动设置环境变量后编译

#### MSVC:
```powershell
# 在 PowerShell 中
$env:PATH = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64;$env:PATH"
cl.exe /EHsc main.cpp
.\main.exe
```

#### MinGW64:
```powershell
# 在 PowerShell 中
$env:PATH = "C:\Program Files (x86)\Dev-Cpp\MinGW64\bin;$env:PATH"
g++.exe -o main.exe main.cpp
.\main.exe
```

## 编译选项说明

### MSVC (cl.exe) 常用选项：
- `/EHsc` - 启用C++异常处理
- `/Fe:filename.exe` - 指定输出文件名
- `/O2` - 优化代码
- `/W4` - 显示所有警告

### MinGW64 (g++.exe) 常用选项：
- `-o filename.exe` - 指定输出文件名
- `-std=c++17` - 使用C++17标准
- `-O2` - 优化代码
- `-Wall` - 显示所有警告
- `-g` - 包含调试信息

## 示例

编译并运行你的 Hello World 程序：

```bash
# 使用 MSVC
.\build_msvc.bat main.cpp

# 或使用 MinGW64
.\build_mingw.bat main.cpp
```

## 常见问题

**Q: 为什么需要设置环境变量？**
A: 编译器不在系统PATH中，需要告诉系统在哪里找到它们。

**Q: 两个编译器有什么区别？**
A: MSVC是微软的编译器，更适合Windows开发；MinGW是GCC的Windows版本，更适合跨平台项目。

**Q: 我应该用哪个？**
A: 初学者建议用MinGW64（更简单），Windows专业开发用MSVC。
