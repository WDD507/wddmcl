# 使用 MinGW64 编译 C++ 程序（支持多文件）
param(
    [string]$OutputFile = "launcher.exe"
)

$MINGW_PATH = "C:\Program Files (x86)\Dev-Cpp\MinGW64\bin"
$env:PATH = "$MINGW_PATH;$env:PATH"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "使用 MinGW64 (g++) 编译器编译程序" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 收集所有源文件
$sourceFiles = @()
$sourceFiles += "main.cpp"
$sourceFiles += "launcher.cpp"
$sourceFiles += Get-ChildItem -Path "minecraft" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }
$sourceFiles += Get-ChildItem -Path "account" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }
$sourceFiles += Get-ChildItem -Path "java" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }
$sourceFiles += Get-ChildItem -Path "mod" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }
$sourceFiles += Get-ChildItem -Path "utils" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }
$sourceFiles += Get-ChildItem -Path "config" -Filter "*.cpp" -Recurse | ForEach-Object { $_.FullName }

Write-Host "找到 $($sourceFiles.Count) 个源文件" -ForegroundColor Green
Write-Host ""

# 构建编译命令
$includePaths = @(
    "-I.",
    "-I./minecraft",
    "-I./account",
    "-I./java",
    "-I./mod",
    "-I./utils",
    "-I./config",
    "-I./third_party"
)

$compilerArgs = $includePaths + $sourceFiles + @("-o", $OutputFile, "-std=c++17")

Write-Host "正在编译..." -ForegroundColor Yellow
& "g++.exe" $compilerArgs 2>&1 | Out-String | Write-Host

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "编译成功！" -ForegroundColor Green
    Write-Host "可执行文件: $OutputFile" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    
    $choice = Read-Host "是否运行程序？(Y/N)"
    if ($choice -eq "Y" -or $choice -eq "y") {
        Write-Host ""
        Write-Host "运行程序:" -ForegroundColor Cyan
        Write-Host "----------------------------------------" -ForegroundColor Gray
        & ".\$OutputFile"
        Write-Host "----------------------------------------" -ForegroundColor Gray
    }
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "编译失败！请检查代码错误。" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
}
