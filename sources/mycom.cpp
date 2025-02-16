#include <mycom.h>
#include <QElapsedTimer>

QSerialPort MyCom;//串口对象，项目中唯一的串口对象
QByteArray MyComRevBUff;//串口接收数据缓存

void delay_msec(int msec)
{
    QTime MyTimer;
    MyTimer.start();
    while (MyTimer.elapsed()<msec) QCoreApplication::processEvents();


}
void delay_usec(qint64 usec)
{
    QElapsedTimer MyTimer1;
    qint64 nasec = 1000*usec;
    MyTimer1.start();
    while (MyTimer1.nsecsElapsed()<nasec) QCoreApplication::processEvents();
}
