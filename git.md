```shell
创建远端仓库
git init --bare

生成公钥和私钥
ssh-keygen -t rsa

拷贝客户端公钥到~/.ssh/authorized_keys

在文件夹下面设置
git config user.name qiao
git config user.email qiao@163.com

全局设置
git config --global user.name qiao
git config --global user.email qiao@163.com

配置写在了.gitconfig里面
```

```shell
放到暂存区，可以分多次提交
git add filename

提交到本地仓库，可以加具体文件名
git commit add.cc api.h -m "feat:add func"

origin-远端服务器的别名，将本地仓库的数据推送到远端仓库
git push origin master

将远端仓库的数据同步到本地工作目录
git pull = git getch + git merge
git pull

查看提交日志
git log

将远程仓库所包含分支的最新commit-id记录到本地
git fetch

撤回提交到暂存区的操作
git restore -S filename

撤回提交到本地仓库的操作
git reset --soft commit-id/head
git reset --mixed head^
git reset --soft 退到暂存区 --mixed 退到工作区  --hard 直接删掉

清空工作区的修改
git checkout . 添加的文件不受影响，修改的文件会回退
git checkout -f .
```

```shell
本地仓库的整理操作

补充上一次提交
git commit --amend   

整理多次提交
git rebase -i h1 h2  左开右闭
= git commit --amend + git rebase --continue
```


```shell
分支操作
git branch -a
git branch

创建分支
在本地创建一个分支
git branch 本地分支名字

在本地创建一个分支，并切换分支
git checkout -b 本地分支名字

切换分支
git checkout 分支名字
git switch 分支名字

合并分支
git merge 

解决冲突
<<<<<<<<<HEAD
==========
>>>>>>>>>>commit
git add .
git commit . -i -m "fix:merge master"

解决冲突 git rebase
git rebase master 产生冲突，状态变为rebase
<<<<<<<<<HEAD
==========
>>>>>>>>>>commit
git add .
git rebase --continue 解决rebase状态
没有增加提交点

```