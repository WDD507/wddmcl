# Git 使用手册

## 1. 初始配置（只需执行一次）

```bash
# 设置用户名和邮箱（提交记录会显示这些信息）
git config --global user.name "你的用户名"
git config --global user.email "你的邮箱@example.com"

# 设置默认分支名为 main
git config --global init.defaultBranch main

# 查看当前配置
git config --list
```

## 2. 仓库初始化

```bash
# 在当前目录初始化一个新仓库
git init

# 克隆远程仓库到本地
git clone https://github.com/用户名/仓库名.git

# 克隆到指定目录
git clone https://github.com/用户名/仓库名.git 目标目录名
```

## 3. 文件操作

```bash
# 查看当前状态（哪些文件被修改、哪些未跟踪）
git status

# 添加单个文件到暂存区
git add 文件名

# 添加所有修改到暂存区
git add .

# 添加某个目录下的所有修改
git add 目录名/

# 从暂存区移除（取消 add，但不删除文件）
git restore --staged 文件名

# 从仓库和工作目录同时删除文件
git rm 文件名

# 从仓库删除但保留本地文件
git rm --cached 文件名
```

## 4. 提交

```bash
# 提交暂存区的内容
git commit -m "提交说明"

# 跳过暂存区，直接提交所有已跟踪文件的修改
git commit -am "提交说明"

# 修改最近一次提交的说明（不产生新提交）
git commit --amend -m "新的提交说明"

# 查看提交历史
git log

# 查看简洁的提交历史（一行一条）
git log --oneline

# 查看最近 N 条提交
git log --oneline -5
```

## 5. 远程仓库

```bash
# 查看已配置的远程仓库
git remote -v

# 添加远程仓库
git remote add origin https://github.com/用户名/仓库名.git

# 修改远程仓库地址
git remote set-url origin https://github.com/新用户名/新仓库名.git

# 重命名远程仓库
git remote rename origin github

# 删除远程仓库
git remote remove origin
```

## 6. 推送与拉取

```bash
# 首次推送并关联远程分支（-u = --set-upstream）
git push -u origin main

# 之后推送（不再需要 -u）
git push

# 推送到指定远程仓库
git push origin main

# 强制推送（覆盖远程历史，谨慎使用）
git push --force

# 拉取远程更新并合并到当前分支
git pull

# 拉取远程更新但不合并（只更新远程跟踪分支）
git fetch

# 拉取远程更新并允许合并不相关的历史
git pull origin main --allow-unrelated-histories
```

## 7. 分支操作

```bash
# 查看所有本地分支
git branch

# 查看所有分支（含远程）
git branch -a

# 创建新分支
git branch 分支名

# 切换到指定分支
git checkout 分支名
# 或（新版写法）
git switch 分支名

# 创建并切换到新分支
git checkout -b 分支名
# 或
git switch -c 分支名

# 重命名当前分支
git branch -M 新分支名

# 删除本地分支（需先切换到其他分支）
git branch -d 分支名

# 强制删除本地分支
git branch -D 分支名

# 删除远程分支
git push origin --delete 分支名
```

## 8. 撤销操作

```bash
# 撤销工作区的修改（还没 add）
git restore 文件名
# 或（旧写法）
git checkout -- 文件名

# 撤销暂存区的修改（已 add，还没 commit）
git restore --staged 文件名
# 或（旧写法）
git reset HEAD 文件名

# 撤销最近一次提交，保留修改在工作区
git reset --soft HEAD~1

# 撤销最近一次提交，修改放回暂存区
git reset --mixed HEAD~1

# 撤销最近一次提交，删除修改（危险！）
git reset --hard HEAD~1

# 回退到指定提交（保留修改在工作区）
git reset --soft 提交哈希

# 回退到指定提交（彻底丢弃之后的修改）
git reset --hard 提交哈希
```

## 9. 合并与变基

```bash
# 将指定分支合并到当前分支
git merge 分支名

# 变基：将当前分支的提交移植到指定分支之上
git rebase 分支名

# 解决冲突后继续 rebase
git rebase --continue

# 取消 rebase
git rebase --abort
```

## 10. 标签（版本发布）

```bash
# 查看所有标签
git tag

# 创建轻量标签
git tag v1.0.0

# 创建附注标签（推荐，包含说明信息）
git tag -a v1.0.0 -m "版本说明"

# 推送单个标签到远程
git push origin v1.0.0

# 推送所有标签到远程
git push origin --tags

# 删除本地标签
git tag -d v1.0.0

# 删除远程标签
git push origin --delete v1.0.0
```

## 11. 储藏（临时保存修改）

```bash
# 临时保存当前工作区的修改
git stash

# 查看所有储藏
git stash list

# 恢复最近一次储藏（保留储藏记录）
git stash apply

# 恢复最近一次储藏并删除储藏记录
git stash pop

# 删除最近一次储藏
git stash drop

# 清空所有储藏
git stash clear
```

## 12. .gitignore 文件

```bash
# .gitignore 语法：
# *.exe          忽略所有 .exe 文件
# /build/        忽略根目录的 build 文件夹
# node_modules/  忽略所有位置的 node_modules 文件夹
# !test.exe      取消忽略（前面的 ! 表示反选）
# *.log          忽略所有 .log 文件
# temp*.txt      忽略以 temp 开头的 .txt 文件

# 如果文件已被跟踪，.gitignore 不会生效，需要先移除跟踪：
git rm --cached 文件名
```

## 13. 多远程仓库管理

```bash
# 添加多个远程仓库（例如同时推送到 GitHub 和 Gitee）
git remote add github https://github.com/用户名/仓库名.git
git remote add gitee https://gitee.com/用户名/仓库名.git

# 分别推送
git push github main
git push gitee main

# 设置一个远程同时推送多个目标
git remote set-url --add --push origin https://github.com/用户名/仓库名.git
git remote set-url --add --push origin https://gitee.com/用户名/仓库名.git
# 之后 git push 会同时推送到两个仓库
```

## 14. 常用快捷操作

```bash
# 查看修改了哪些文件
git status --short

# 查看具体修改内容
git diff

# 查看已暂存的修改内容
git diff --cached

# 查看某个文件的提交历史
git log --oneline -- 文件名

# 查看某次提交修改了哪些文件
git show --stat 提交哈希

# 临时查看某个历史版本（不创建分支）
git checkout 提交哈希
# 返回最新状态
git checkout main

# 查找引入某行代码的提交
git blame 文件名
```

## 15. 认证配置

```bash
# GitHub：使用 Personal Access Token 代替密码
# 1. 在 GitHub 网页 Settings → Developer settings → Personal access tokens → Generate new token
# 2. 勾选 repo 权限
# 3. 复制生成的 token

# 推送时输入：
#   Username: 你的 GitHub 用户名
#   Password: 粘贴刚才的 token（不是账号密码）

# 缓存凭据（Windows 自带 Credential Manager）
git config --global credential.helper manager

# 或使用 Git Credential Manager（推荐）
git config --global credential.helper manager-core
```

## 16. 常见问题

### Q: push 被拒绝，提示 fetch first
远程有你本地没有的提交。选择：
```bash
# 方案 A：拉取并合并（保留远程内容）
git pull origin main --allow-unrelated-histories
git push

# 方案 B：强制覆盖远程（丢弃远程内容）
git push --force
```

### Q: commit message 写错了
```bash
git commit --amend -m "正确的说明"
# 如果已经推送，需要强制推送
git push --force
```

### Q: 不小心 git reset --hard，代码丢了
```bash
# 查看所有操作记录（包括被 reset 的）
git reflog

# 找到丢失的提交哈希，恢复
git reset --hard 提交哈希
```

### Q: 想忽略已被跟踪的文件
```bash
git rm --cached 文件名
echo "文件名" >> .gitignore
git add .gitignore
git commit -m "停止跟踪 文件名"
```

### Q: 远程仓库 origin 已存在，想换地址
```bash
# 直接修改地址
git remote set-url origin https://github.com/新用户名/新仓库名.git

# 或先删后加
git remote remove origin
git remote add origin https://github.com/新用户名/新仓库名.git
```

### Q: 想同时推送多个远程仓库
```bash
# 方法 1：分别推送
git push origin main
git push gitee main

# 方法 2：配置多推送目标
git remote set-url --add --push origin https://github.com/用户名/仓库名.git
git remote set-url --add --push origin https://gitee.com/用户名/仓库名.git
git push
```
