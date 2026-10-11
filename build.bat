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

REM 确保目标文件夹已存在
if not exist build\core mkdir build\core
if not exist build\awtk mkdir build\awtk
if not exist build\zlib mkdir build\zlib

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
REM for %%F in (third_party\zlib\zlib-1.3.2\contrib\minizip\*.c) do set "zlib=!zlib! %%F"
REM set "zlibcontrib="

echo 正在复制动态库...
copy third_party\awtk\bin\awtk.dll .\ >nul
copy third_party\awtk\bin\tkc.dll .\ >nul
copy third_party\libcpr\bin\libcurl-x64.dll .\ >nul
echo 动态库复制完成。
echo.

echo 正在编译...
echo.

REM 编译命令
REM 1.编译核心
echo 正在编译核心...
set "COREFAIL=0"
for %%F in (%SOURCES%) do (
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
        -std=c++17 ^
        -c %%F ^
        -o ./build/core/%%~nF.o
    if !ERRORLEVEL! NEQ 0 (
        set "COREFAIL=1"
    )
)
if %COREFAIL% NEQ 0 (
    echo 编译核心失败
)
if %COREFAIL% EQU 0 (
    echo 编译核心成功！
)

REM 2.编译GUI
echo 正在编译GUI...
set "AWTKFAIL=0"
for %%F in (%AWTK_GUI%) do (
    if "%%~xF"==".c" (
        gcc.exe ^
            -I./third_party ^
            -I./src ^
            -I./third_party/awtk/include_flat ^
            -I./third_party/libcpr/include/cpr ^
            -I./third_party/libcpr/include ^
            -I./third_party/zlib/zlib-1.3.2 ^
            -I./third_party/zlib/zlib-1.3.2/contrib/minizip ^
            -std=c++17 ^
            -c %%F ^
            -o ./build/awtk/%%~nF.o
    ) else (
        g++.exe ^
            -I./third_party ^
            -I./src ^
            -I./third_party/awtk/include_flat ^
            -I./third_party/libcpr/include/cpr ^
            -I./third_party/libcpr/include ^
            -I./third_party/zlib/zlib-1.3.2 ^
            -I./third_party/zlib/zlib-1.3.2/contrib/minizip ^
            -std=c++17 ^
            -c %%F ^
            -o ./build/awtk/%%~nF.o
    )
    if !ERRORLEVEL! NEQ 0 (
        set "AWTKFAIL=1"
    )
)
if %AWTKFAIL% NEQ 0 (
    echo 编译GUI失败
)
if %AWTKFAIL% EQU 0 (
    echo 编译GUI成功！
)

REM 3.编译zlib (注意：必须使用gcc！！！)
echo 正在编译zlib...
set "ZLIBFAIL=0"
for %%F in (%zlib%) do (
    gcc.exe ^
        -Ithird_party/zlib/zlib-1.3.2 ^
        -Ithird_party/zlib/zlib-1.3.2/contrib/minizip ^
        -c %%F ^
        -o ./build/zlib/%%~nF.o
    if !ERRORLEVEL! NEQ 0 (
        set "ZLIBFAIL=1"
    )
)
for %%F in (
    third_party\zlib\zlib-1.3.2\contrib\minizip\zip.c
    third_party\zlib\zlib-1.3.2\contrib\minizip\unzip.c
    third_party\zlib\zlib-1.3.2\contrib\minizip\ioapi.c
) do (
    gcc.exe ^
        -Ithird_party/zlib/zlib-1.3.2 ^
        -Ithird_party/zlib/zlib-1.3.2/contrib/minizip ^
        -c %%F ^
        -o ./build/zlib/%%~nF.o
    if !ERRORLEVEL! NEQ 0 (
        set "ZLIBFAIL=1"
    )
)
if %ZLIBFAIL% NEQ 0 (
    echo 编译zlib失败
)
if %ZLIBFAIL% EQU 0 (
    echo 编译zlib成功！
)

REM 4.链接
echo 正在链接...
set "OBJS="
for %%F in (build\core\*.o) do set "OBJS=!OBJS! %%F"
for %%F in (build\awtk\*.o) do set "OBJS=!OBJS! %%F"
for %%F in (build\zlib\*.o) do set "OBJS=!OBJS! %%F"
g++.exe ^
    -Lthird_party/libcpr/lib ^
    -Lthird_party/awtk/bin ^
    %OBJS% ^
    -lawtk -ltkc -lcpr -lcurl -lm ^
    -o ./build/launcher.exe

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
