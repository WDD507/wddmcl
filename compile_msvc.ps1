# 使用 MSVC 编译 C++ 程序
param(
    [string]$SourceFile = "main.cpp"
)

$MSVC_BASE = "C:\Program Files\Microsoft Visual Studio\2022\Community"
$MSVC_BIN = "$MSVC_BASE\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64"
$MSVC_INCLUDE = "$MSVC_BASE\VC\Tools\MSVC\14.44.35207\include"
$WINDOWS_SDK_INCLUDE = "C:\Program Files (x86)\Windows Kits\10\Include"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "使用 MSVC (cl) 编译器编译程序" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

if (-not (Test-Path $SourceFile)) {
    Write-Host "错误: 找不到文件 $SourceFile" -ForegroundColor Red
    Write-Host "用法: .\compile_msvc.ps1 [source_file.cpp]" -ForegroundColor Yellow
    exit 1
}

# 查找最新的 Windows SDK
$sdkVersions = Get-ChildItem $WINDOWS_SDK_INCLUDE -Directory | Where-Object { $_.Name -match '^\d+\.\d+\.\d+\.\d+$' } | Sort-Object Name -Descending
if ($sdkVersions.Count -gt 0) {
    $latestSdk = $sdkVersions[0].Name
    $WINDOWS_SDK_INCLUDE_VERSION = "$WINDOWS_SDK_INCLUDE\$latestSdk"
} else {
    Write-Host "警告: 未找到 Windows SDK，编译可能失败" -ForegroundColor Yellow
    $WINDOWS_SDK_INCLUDE_VERSION = $WINDOWS_SDK_INCLUDE
}

$FileName = [System.IO.Path]::GetFileNameWithoutExtension($SourceFile)
$OutputExe = "$FileName.exe"

Write-Host "正在编译: $SourceFile" -ForegroundColor Green

# 设置环境变量并编译
$env:INCLUDE = "$MSVC_INCLUDE;$WINDOWS_SDK_INCLUDE_VERSION\ucrt;$WINDOWS_SDK_INCLUDE_VERSION\um;$WINDOWS_SDK_INCLUDE_VERSION\shared"
$env:LIB = ""

& "$MSVC_BIN\cl.exe" /EHsc /Fe:"$OutputExe" $SourceFile

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "编译成功！" -ForegroundColor Green
    Write-Host "可执行文件: $OutputExe" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    
    $choice = Read-Host "是否运行程序？(Y/N)"
    if ($choice -eq "Y" -or $choice -eq "y") {
        Write-Host ""
        Write-Host "运行程序:" -ForegroundColor Cyan
        Write-Host "----------------------------------------" -ForegroundColor Gray
        & ".\$OutputExe"
        Write-Host "----------------------------------------" -ForegroundColor Gray
    }
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "编译失败！请检查代码错误。" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
}
