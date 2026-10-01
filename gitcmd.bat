@echo off
setlocal enabledelayedexpansion

REM Git 全局设置
git config --global user.name "WDDwthclj"
git config --global user.email "1751398270@qq.com"
git config --global user.name "WDD507"
git config --global user.email "1751398270@qq.com"

REM 创建 git 仓库
git init 
touch README.md
git add README.md
git commit -m "first commit"
git remote add origin https://gitee.com/user-wddwthclj/wddmcl.git
git push -u origin "master"

REM 已有仓库
REM cd existing_git_repo
git remote add origin https://gitee.com/user-wddwthclj/wddmcl.git
git push -u origin "master"


git -C E:\PCL\wddmcl init
git -C E:\PCL\wddmcl add .
git -C E:\PCL\wddmcl commit -m "初始提交：WDD Minecraft Launcher v0.0.5.1 Beta"

git -C E:\PCL\wddmcl remote add gitee https://github.com/WDD507/wddmcl.git
git -C E:\PCL\wddmcl remote set-url gitee https://github.com/WDD507/wddmcl.git
git -C E:\PCL\wddmcl branch -M main
git -C E:\PCL\wddmcl push -u origin main

git -C E:\PCL\wddmcl remote add origin https://github.com/WDD507/wddmcl.git
git -C E:\PCL\wddmcl remote set-url origin https://github.com/WDD507/wddmcl.git
git -C E:\PCL\wddmcl branch -M main
git -C E:\PCL\wddmcl push -u origin main
