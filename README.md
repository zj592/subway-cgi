# 深圳地铁查询系统（CGI 实现）

《数据结构与算法》实训实验项目。基于 **Dijkstra 最短路径算法**，提供 Web 端的地铁换乘 / 最短耗时查询。

## 功能

- 输入起点站、终点站，返回最短耗时路线与换乘方案
- 按线路号着色展示途经站点
- 同时提供 **CGI（网页）版** 与 **控制台（命令行）版** 两套实现

## 目录结构

```
.
├── subway_cgi.cgi        # 预编译 CGI 程序（Windows x86，MinGW 编译）
├── subway_query.html     # 前端查询页面（Tailwind CSS）
├── stations.txt          # 站点数据（id,name）
├── connections.txt       # 站点连接（from,to,line,time）
├── line.txt              # 线路定义（id,name,color）
└── src/
    ├── subway_cgi.c      # CGI 版源码（含 main，编译产物即 subway_cgi.cgi）
    ├── main.c            # 控制台版入口
    ├── subway.c          # 地铁网络数据结构与算法实现
    └── subway.h          # 头文件
```

## 构建（MinGW GCC，Windows）

```bash
# CGI 版：编译为 subway_cgi.cgi
gcc -o subway_cgi.cgi src/subway_cgi.c

# 控制台版：交互式菜单
gcc -o subway_console.exe src/main.c src/subway.c
```

> 控制台版运行时需要 `stations.txt` / `connections.txt` / `line.txt` 与可执行文件同目录。
> 源码 `main.c` 中以 `lines.txt` 之名加载线路定义，若文件名不同请相应改名后再运行。

## 部署运行（Web 版）

1. 安装 Apache HTTP Server 并开启 CGI 模块。
2. 将 `subway_cgi.cgi` 与数据文件 `stations.txt`、`connections.txt` 放入 Apache 的 `cgi-bin` 目录。
   （源码默认读取 `C:/Apache24/cgi-bin/` 路径，可按实际部署目录修改 `src/subway_cgi.c` 中的路径后再编译。）
3. 配置站点监听端口 `81`——前端 `subway_query.html` 的查询表单提交到
   `http://localhost:81/cgi-bin/subway_cgi.cgi`。
4. 浏览器打开 `subway_query.html`，输入起点 / 终点站即可查询路线。

## 数据格式

| 文件             | 字段                    | 说明                         |
| ---------------- | ----------------------- | ---------------------------- |
| `stations.txt`   | `id,name`               | 站点编号, 站点名称           |
| `connections.txt`| `from,to,line,time`     | 相邻站点编号, 线路号, 区间耗时（分钟） |
| `line.txt`       | `id,name,color`         | 线路号, 线路名称, 线路颜色   |

## 说明

- 仓库附带一份**预编译的 Windows 二进制** `subway_cgi.cgi`，可直接部署；如需修改算法或页面，
  请用上方命令从 `src/` 重新编译。
- 演示视频因体积过大（约 30MB）未纳入版本管理，本地 `*.mp4` 已被 `.gitignore` 忽略。

## 作者

zj592 —— 课程实训作业开源整理。
