#ifndef MYCOM_H
#define MYCOM_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <qapplication>

//声明全局对象，在.cpp文件定义（切记）
extern QSerialPort MyCom;//串口对象，项目中唯一的串口对象
extern QByteArray MyComRevBUff;//串口接收数据缓存

extern void delay_msec(int msec);//非阻塞延时函数

#endif // MYCOM_H
