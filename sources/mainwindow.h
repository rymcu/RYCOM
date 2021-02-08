#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QTime>
#include <QFileDialog>
#include <QFile>
#include <QTextCodec>
#include <QScreen>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void MyComRevSlot();//串口接收槽函数
    void portRecvDataDelay();// 串口接收数据延时

    void on_radioButton_clicked();//显示多行发送界面

    void on_checkBoxPeriodicSend_stateChanged(int arg1);//周期发送，和多行发送公用一个定时器，选中则打开定时器开始周期发送数据
    void on_checkBoxPeriodicMutiSend_stateChanged(int arg1);//多行循环发送

    void on_checkBoxReVTime_stateChanged(int arg1);//设置是否显示接收时间的标志位
    void on_pushButtonOpen_clicked(); //打开或关闭串口按钮槽函数
    void on_pushButtonSend_clicked();//发送按钮槽函数

    void on_pushButtonClearRev_clicked();//清空接收区按钮槽函数
    void on_pushButtonClearSend_clicked();//清空发送区按钮槽函数
    void on_pushButtonStopRev_clicked();//接收区停止显示槽函数

    void on_pushButtonRdFile_clicked();//读取文件,并显示在发送窗口
    void on_pushButtonSaveRev_clicked();//保存文件按钮槽函数

    //多行发送按钮1-10槽函数
    void on_pushButtonMuti1_clicked();
    void on_pushButtonMuti2_clicked();
    void on_pushButtonMuti3_clicked();
    void on_pushButtonMuti4_clicked();
    void on_pushButtonMuti5_clicked();
    void on_pushButtonMuti6_clicked();
    void on_pushButtonMuti7_clicked();
    void on_pushButtonMuti8_clicked();
    void on_pushButtonMuti9_clicked();
    void on_pushButtonMuti10_clicked();

    void on_pushButtonMutiReset_clicked();//清除多行文本

private:
    Ui::MainWindow *ui;

    //添加自定义变量
    QSerialPort MyCom;//串口对象，项目中唯一的串口对象
    long ComSendSum,ComRevSum;//发送和接收流量统计变量

    bool StopDis = false;//停止显示标识
    bool TimeDateDisp = false;//显示时间标志
    bool MutiState[10];//多行选中状态
    int LastSend = 0;//多行循环发送起始位置

    QTimer *PriecSendTimer;//周期发送定时器，和多行的循环发送共用
    QTimer *recvDelayTimer;//接收延时定时器，解决中文分段乱码

    QDateTime curDateTime;//系统时间变量

    //下列标签将显示在状态栏
    QLabel *qlbSendSum,*qlbRevSum;//发送接收流量label对象
    QLabel *currentTimeLabel; //系统时间显示标签
    QLabel *qlbLinkRYMCU;//官网链接标签对象
    QLabel *qlbLinkSource;//源码链接标签对象

    void Pre_on_pushButtonSend_clicked();//多行周期发送预处理函数
    int Get_checkBoxMuti_State();//获取多行周期发送的选中状态
    void SendDataByNoOfEditLineNo(int EditLineNo);//根据状态,发送指定行的数据
    void time_update(); //时间更新槽函数，状态栏显示时间

    void setNumOnLabel(QLabel *lbl, QString strS, long num);//设置标签内容

    //文件打开、保存、编码转换函数
    bool openTextByIODevice(const QString &aFileName);
    bool saveTextByIODevice(const QString &aFileName);
    QString byteArrayToUnicode(const QByteArray &array);//编码格式转换

    void changeObjectSize(const QObject &o, double objectRate);//调整控件大小
    void refreshDPI();//刷新界面
    QScreen* screen;
    double myobjectRate;//dpi比例，用于调整主窗口的大小
};

#endif // MAINWINDOW_H
