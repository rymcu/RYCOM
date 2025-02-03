#include "mainwindow.h"
#include <QApplication>
#include <ryisp.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/************************************************
 ********************版本说明*********************
 * 20250202 RYCOM 2.6.0.exe
 * 1.添加ESP32系列芯片串口下载程序，支持从0x0开始下载合并后的单个.bin
 * 2.支持bootloader.bin,partition-table.bin,app.bin分段下载
 * 3.自动识别ESP32芯片信号和flash大小
 *
 * 20240323 RYCOM 2.5.1.exe
 * 1.修复重复点击Open按钮卡死bug
 *
 * 20240320 RYCOM2.5.exe
 * 1.增加串口下载Flash擦除选项;
 * 2.修复.Hex文件打开时下载地址错误;
 *3.修复指示灯bug及其他。
 *
 * 20240320 RYCOM2.4.exe
 * 1.增加可自定义串口波特率;
 * 2.增加串口指示灯;
 * 3.更改APP图标。
 *
 * 20240310 RYCOM2.3.exe
 * 1.优化STM32串口下载程序，支持.bin和.hex格式，支持STM32F1,STM32F4系列；
 * 2.支持串口ISP一键下载，支持目标文件更新自动启动串口ISP。
 *
 * 20220730 RYCOM2.2.exe/dmg
 * 1.增加串口Ymodem发送文件功能
 * 2.优化界面文字大小
 *
 * 20220724 RYCOM2.1.exe/dmg
 * 1.增加串口高波特率支持
 * 2.去除MAC端串口列表重复显示
 *
 * 20211215 RYCOM2.0.exe/dmg
 * 1.修复发送十六进制0x00错误问题；
 * 2.增加STM32串口下载程序功能，支持.bin和.hex格式，支持STM32F1,STM32F4系列；
 * 3.STM32串口ISP操作手册，https://www.rymcu.com/article/261;
 *
 * 20210208 RYCOM1.0.exe/dmg
 * 1.支持基本串口收、发功能；;
 * 2.支持周期发送，十六进制发送、接收等功能;
 **********************************************/
