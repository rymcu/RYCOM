#include "mycombobox.h"

myComboBox::myComboBox(QWidget *parent) : QComboBox(parent)
{
    // 扫描可用串口
    scanActivePort();
}

// 扫描可用串口
void myComboBox::scanActivePort()
{
    // 先清空列表项，防止多次刷新后重叠
    clear();

    // 串口端口号列表
    QStringList serialPortName;

    // 自动扫描当前可用串口，返回值追加到字符数组中
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){

        //serialPortName << info.portName();// 不携带有串口设备信息的文本

        // 携带有串口设备信息的文本
        QString serialPortInfo = info.portName() + ": " + info.description();// 串口设备信息，芯片/驱动名称
        //QString serialPortInfo = info.portName() + ": " + info.manufacturer();// 串口设备制造商
        //QString serialPortInfo = info.portName() + ": " + info.serialNumber();// 串口设备的序列号，没什么用
        //QString serialPortInfo = info.portName() + ": " + info.systemLocation();// 串口设备的系统位置，没什么用
        serialPortName << serialPortInfo;
    }
    // 可用串口号，显示到串口选择下拉框中
    this->addItems(serialPortName);
}

// 重写鼠标点击事件
void myComboBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        // 扫描可用串口
        scanActivePort();

        // 弹出下拉框
        showPopup();
    }
}
