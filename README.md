## 💡 简介

RYCOM是一款开源的串口调试助手，使用QT设计，C++编写，可在Windows、Linux、MacOS操作系统上使用。支持STM32串口下载程序。
# RYCOM

 #### 20250214 RYCOM 2.6.1.exe
 
- 1.修复MAC下进度条不显示百分比bug
- 2.修复STM32串口下载期间，接收区继续显示串口数据bug
- 3.STM32下载时文件选择模式改成与ESP32一致
- 4.ESP32下载握手失败时，增加处理措施具体步骤
- 5.添加MAC版本图标
- 6.ESP32下载文件选择框记录历史路径
- 7.完善按钮文字显示,界面文字，界面布局局部调整等
- 8.修复ESP32下载更改波特率下载错误bug
- 9.修复ESP32文件框显示/隐藏逻辑错误bug
- 10.添加qDebug条件编译选项
- 11.今天情人节，写个毛的代码
 
 #### 20250202 RYCOM 2.6.0.exe

- 1.添加ESP32系列芯片串口下载程序，支持从0x0开始下载合并后的单个.bin;
- 2.支持bootloader.bin,partition-table.bin,app.bin分段下载;
- 3.自动识别ESP32芯片信号和flash大小;
- 4.ESP32串口下载使用方法https://rymcu.com/article/2203

 #### 20240323 RYCOM 2.5.1.exe

- 1.修复重复点击Open按钮卡死bug。

 #### 20240320 RYCOM2.5.exe

- 1.增加串口下载Flash擦除选项;
- 2.修复.Hex文件打开时下载地址错误;
- 3.修复指示灯bug及其他。
 #### 20240320 RYCOM2.4.exe
- 1.增加可自定义串口波特率;
- 2.增加串口指示灯;
- 3.更改APP图标。
 #### 20240310 RYCOM2.3.exe
- 1.优化STM32串口下载程序，支持.bin和.hex格式，支持STM32F1,STM32F4系列；
- 2.支持串口ISP一键下载，支持目标文件更新自动启动串口ISP。
#### 20220730 RYCOM2.2.exe/dmg
- 1.增加串口Ymodem发送文件功能
- 2.优化界面文字大小
#### 20211215 RYCOM2.0.exe
- 1.修复发送十六进制0x00错误问题； 
- 2.增加STM32串口下载程序功能，支持.bin和.hex格式，支持STM32F1,STM32F4系列；
- 3.STM32串口ISP操作手册，https://www.rymcu.com/article/261
#### 20210208 RYCOM1.0.exe/dmg
- 1.支持基本串口收、发功能；
- 2.支持周期发送，十六进制发送、接收等功能。
## ⚡ 动机

受到 [ronger](https://www.rymcu.com/user/ronger) 的跨平台串口助手[nebula-helper](https://www.rymcu.com/user/ronger)启发, 打造一个基于C++的类似项目。

## ✨ 特性

- 基本功能
  - 串口数据发送、接收
  - 十六进制、文本数据发送
  - 十六进制、文本数据显示
  - 支持自动发送换行
  - 数据接收时间显示可选
  - 支持周期循环发送
  - 发送、接收流量统计
  - 支持中文发送、接收
  - 支持文件发送
  - 可保存接收数据
  - 支持多行发送
  - 支持多行循环发送

## 报告缺陷

> 调试助手可能存在一些潜在的缺陷，大家如果有空的话可以帮助我们一起解决。

如果你在使用助手时发现了如下类型的问题，请回帖进行反馈，并附上 bug 截图以及操作步骤：

* **功能性缺陷**：例如数据发送失败、数据显示乱码等
* **安全性漏洞**：例如串口打开失败、软件自动退出等

## 功能建议

欢迎对RYCOM串口调试小助手提出功能特性方面的建议，我们一起讨论，如果有可能我们会尽快实现。

## 鸣谢
- 感谢 `JetBrains` 对本项目的帮助,为作者提供了开源许可版 `JetBrains` 全家桶
![JetBrains](src/main/resources/static/jetbrains.png)
