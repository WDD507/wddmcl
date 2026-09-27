@echo off
chcp 65001 >nul
echo ========================================
echo 下载第三方库
echo ========================================
echo.

REM 创建目录
if not exist "third_party" mkdir third_party
if not exist "third_party\nlohmann" mkdir third_party\nlohmann
if not exist "third_party\cpr" mkdir third_party\cpr

echo [1/2] 下载 nlohmann/json...
echo 请访问: https://github.com/nlohmann/json/releases
echo 下载最新的 single_include.zip
echo 解压后将 json.hpp 放到: third_party\nlohmann\
echo.
pause

echo.
echo [2/2] 下载 cpr (C++ Requests)...
echo 请访问: https://github.com/libcpr/cpr/releases
echo 下载最新的 Windows 版本
echo 或者使用 vcpkg: vcpkg install cpr
echo.
pause

echo.
echo 完成！请确保文件已正确放置。
pause
