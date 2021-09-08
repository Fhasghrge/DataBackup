# 数据备份（软件开发综合实验）

## 技术栈： 
- GUI 使用前端 HTML、CSS、JavaScript
- 本地服务使用C++编写服务 / 时间充裕的话可以加上Node编写服务
- 前后端通信使用 webSocket

## 开发规范
1. 使用`gitflow`模式开发，新的功能需要创建feature分支开发，开发完成合并到main分支

2. 提交代码commit编写规范：[commit 规范](https://www.zhihu.com/question/21209619)， 使用`git rebase` 代替 `git merge`

3. 注意需要在每个模块中注明开发环境的版本， 并且编写好`build.sh`脚本

4. 开发想法/讨论问题 统一放在github的**issues**中管理

5. 注意使用`.ignore`忽略不必要的代码提交