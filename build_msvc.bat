@echo off
REM 使用 MSVC 编译器编译 C++ 程序
REM 设置 MSVC 环境变量
set "MSVC_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64"
set "PATH=%MSVC_PATH%;%PATH%"

echo ========================================
echo 使用 MSVC 编译器编译程序
echo ========================================
echo.

REM 检查源文件是否存在
if not exist "%~1" (
    echo 错误: 找不到文件 %~1
    echo 用法: build_msvc.bat ^<source_file.cpp^>
    exit /b 1
)

REM 获取文件名（不含扩展名）
for %%F in ("%~1") do set "FILENAME=%%~nF"

echo 正在编译: %~1
cl.exe /EHsc /Fe:"%FILENAME%.exe" "%~1"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo 编译成功！
    echo 可执行文件: %FILENAME%.exe
    echo ========================================
    echo.
    echo 是否运行程序？(Y/N)
    set /p RUN_CHOICE=
    if /i "%RUN_CHOICE%"=="Y" (
        echo.
        echo 运行程序:
        echo ----------------------------------------
        "%FILENAME%.exe"
        echo ----------------------------------------
    )
) else (
    echo.
    echo ========================================
    echo 编译失败！请检查代码错误。
    echo ========================================
)

pause
