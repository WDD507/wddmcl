@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo 使用 MinGW64 编译器编译 Minecraft Launcher
echo ========================================
echo.

REM 设置MinGW路径
set "MINGW_PATH=C:\Program Files (x86)\Dev-Cpp\MinGW64\bin"
set "PATH=%MINGW_PATH%;%PATH%"

REM 收集所有源文件
set "SOURCES=main.cpp launcher.cpp app.cpp"

for %%F in (minecraft\*.cpp) do set "SOURCES=!SOURCES! %%F"
for %%F in (account\*.cpp) do set "SOURCES=!SOURCES! %%F"
for %%F in (java\*.cpp) do set "SOURCES=!SOURCES! %%F"
for %%F in (mod\*.cpp) do set "SOURCES=!SOURCES! %%F"
for %%F in (utils\*.cpp) do set "SOURCES=!SOURCES! %%F"
for %%F in (config\*.cpp) do set "SOURCES=!SOURCES! %%F"

REM AWTK
set "AWTK_GUI=src\mutually.cpp src\application.c"
for %%F in (src\pages\*.c) do set "AWTK_GUI=!AWTK_GUI! %%F"
for %%F in (src\common\*.c) do set "AWTK_GUI=!AWTK_GUI! %%F"

REM zlib
set "zlib="
for %%F in (third_party\zlib\zlib-1.3.2\*.c) do (
    if defined zlib (set "zlib=!zlib! %%F") else (set "zlib=%%F")
)
for %%F in (third_party\zlib\zlib-1.3.2\contrib\minizip\*.c) do set "zlib=!zlib! %%F"

echo 正在复制动态库...
copy third_party\awtk\bin\awtk.dll .\ >nul
copy third_party\awtk\bin\tkc.dll .\ >nul
copy third_party\libcpr\bin\libcurl-x64.dll .\ >nul
echo 动态库复制完成。
echo.

echo 正在编译...
echo.

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
    -I./src ^
    -I./third_party/awtk/include_flat ^
    -std=c++17 ^
    %SOURCES% ^
    %AWTK_GUI% ^
    %zlib% ^
    -Ithird_party/libcpr/include/cpr ^
    -Ithird_party/libcpr/include ^
    -Ithird_party/zlib/zlib-1.3.2 ^
    -Ithird_party/zlib/zlib-1.3.2/contrib/minizip ^
    -Lthird_party/libcpr/lib ^
    -Lthird_party/awtk/bin ^
    -lawtk -lcpr -lcurl -lm ^
    -o launcher.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo 编译成功！
    echo "可执行文件: launcher.exe"
    echo ========================================
    echo.
    set /p RUN_CHOICE="是否运行程序？(Y/N): "
    if /i "!RUN_CHOICE!"=="Y" (
        echo.
        echo 运行程序:
        echo ----------------------------------------
        launcher.exe
        echo ----------------------------------------
    )
) else (
    echo.
    echo ========================================
    echo 编译失败！请检查代码错误。状态码：%ERRORLEVEL%
    echo ========================================
)

echo.
pause
chcp 936 >nul
