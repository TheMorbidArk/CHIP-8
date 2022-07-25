# CHIP-8 仿真器

- 本项目在Debian 11.03下开发&运行，使用freeglut处理音频/视频输出和用户输入

- 本项目可运行大多数CHIP-8 Games，如pong、UFO、TANK等

- 本项目使用C/C++开发

## 项目介绍

模拟器是模拟计算机系统（系统 A）的内部设计和功能的计算机程序。

它允许用户在完全不同的计算机系统或体系结构（系统 B）上运行为该特定系统（系统 A）设计的软件。

CHIP-8是 Joseph Weisbecker 在 70 年代开发的虚拟机 (VM)。

用 Chip 8 语言编写的游戏可以轻松地在具有 Chip 8 解释器的系统上运行。

 Chip 8 仿真器可能是最简单的仿真项目,共有35 个两个字节长的操作码,4K 内存,15 个 8 位通用寄存器 V0~VE ,第 16 个寄存器 VF 用于“进位标志”

本项目使用C/C++实现了CHIP-8仿真器,并且可以运行大部分CHIP-8游戏

## 目录介绍

CHIP-8  
│ .gitignore  
│ chip_8.cpp  
│ chip_8.h  
│ main.cpp  
│ Makefile  
│ README.md  
├─img  
└─ROMs

chip_8.cpp chip_8.h -> CHIP-8仿真器系统

main.cpp -> 处理音频/视频输出和用户输入

img -> 存放 .md 文件图片资源

ROMs -> 存放 CHIP-8 游戏资源

## 编译方式

### 安装开发环境

```bash
sudo apt-get install gcc g++ gdb make git build-essential freeglut3 freeglut3-dev binutils-gold
```

### 编译&运行

```bash
make
./main ROMs/[name] # [name] -> 游戏ROM包名
```

## 运行截图

[img/Game1.png · TheMorbidArk/Chip-8 - 码云 - 开源中国 (gitee.com)](https://gitee.com/TheMorbidArk/chip-8/blob/master/img/Game1.png)

[img/Game2.png · TheMorbidArk/Chip-8 - 码云 - 开源中国 (gitee.com)](https://gitee.com/TheMorbidArk/chip-8/blob/master/img/Game2.png)

[img/Game3.png · TheMorbidArk/Chip-8 - 码云 - 开源中国 (gitee.com)](https://gitee.com/TheMorbidArk/chip-8/blob/master/img/Game3.png)
