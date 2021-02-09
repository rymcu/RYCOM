#include "mainwindow.h"
#include "ui_mainwindow.h"

/***********************************************************
 * 窗口构造函数，初始化工作在这里完成！
 * 1.失能和隐藏相应控件
 * 2.创建周期发送、时间显示、延时接收定时器，并初始化
 * 3.串口接收函数关联
 * 4.设置底部状态栏，以及相应的标签初始化
 ***********************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    refreshDPI(); //根据dpi调整窗体大小
    //connect(screen, &QScreen::logicalDotsPerInchChanged, this, &MainWindow::onLogicalDotsPerInchChanged);
        //失能和隐藏相应控件
        ui->pushButtonSend->setEnabled(false);//使相应的按钮不可用
        ui->checkBoxPeriodicSend->setEnabled(false);
        ui->checkBoxPeriodicMutiSend->setEnabled(false);
        ui->lineEditTime->setEnabled(false);
        ui->checkBoxAddNewShift->setEnabled(false);
        ui->checkBoxSendHex->setEnabled(false);
        ui->groupBoxMutiSend->hide();//隐藏多行发送区

        ui->groupBoxRev->setFixedWidth(541*myobjectRate);//根据屏幕分辨率不一样固定接收组的大小
        ui->TextRev->setFixedWidth(521*myobjectRate);//根据屏幕分辨率不一样固定接收窗口的大小
        this->setFixedSize(729*myobjectRate,587*myobjectRate);//根据屏幕分辨率不一样固定主窗口的大小


        //创建周期发送、时间显示、延时接收定时器，并初始化
        PriecSendTimer = new QTimer;
        PriecSendTimer->setInterval(1000);//默认周期1000ms
        connect(PriecSendTimer,&QTimer::timeout,this,[=](){Pre_on_pushButtonSend_clicked();});//定时器槽关联,关联至发送按钮槽函数，即定时到来时，自动发送数据

        QTimer *DateTimer = new QTimer(this);//状态栏显示时间，日期
        connect(DateTimer,&QTimer::timeout,this,[=](){time_update();});
        DateTimer->start(1000); //每隔1000ms发送timeout的信号

        recvDelayTimer = new QTimer(this);//接收延时定时器，解决中文分段乱码
        recvDelayTimer->stop();
        connect(recvDelayTimer,SIGNAL(timeout()),this,SLOT(MyComRevSlot()));//接收延时到达，进行串口接收数据处理

        //串口接收函数关联
        connect(&MyCom,SIGNAL(readyRead()),this,SLOT(portRecvDataDelay()));//串口数据接收过程：系统收到串口数据后，进入接收延时函数portRecvDataDelay(),
                                                                           //函数中停止定时器，再启动定时器，定时时间到了再进入接收处理函数MyComRevSlot()
                                                                           //因此，只有收到数据后，在连续的定时时间内都没有数据到了，才会进行数据处理，否则
                                                                           //一直在接收数据，这是为了解决收中文数据时，分段接收显示乱码问题。
        //设置底部状态栏，以及相应的标签初始化
        QStatusBar *STABar = statusBar();//获取状态栏
        qlbSendSum = new QLabel(this);//创建发送统计标签
        qlbRevSum  = new QLabel(this);//创建接收统计标签
        currentTimeLabel = new QLabel(this); // 创建时间，日期显示标签
        qlbLinkRYMCU = new QLabel(this);//官网链接标签对象
        qlbLinkSource = new QLabel(this);//源码链接标签对象

        qlbLinkRYMCU->setMinimumSize(90, 20);// 设置标签最小大小
        qlbLinkSource->setMinimumSize(90, 20);
        qlbSendSum->setMinimumSize(100, 20);
        qlbRevSum->setMinimumSize(100, 20);
        currentTimeLabel->setMinimumSize(100, 20);

         ComSendSum = 0;
         ComRevSum  = 0;

         setNumOnLabel(qlbSendSum, "Tx: ", ComSendSum);
         setNumOnLabel( qlbRevSum, "Rx: ", ComRevSum);

         STABar->addPermanentWidget(qlbSendSum);// 从右往左依次添加
         STABar->addPermanentWidget(qlbRevSum);
         STABar->addPermanentWidget(currentTimeLabel);

         STABar->addWidget(qlbLinkRYMCU);// 从左往右依次添加
         STABar->addWidget(qlbLinkSource);

         qlbLinkRYMCU->setOpenExternalLinks(true);//状态栏显示官网、源码链接
         qlbLinkRYMCU->setText("<style> a {text-decoration: none} </style> <a href=\"https://rymcu.com\">--RYMCU官网--");// 无下划线
         qlbLinkSource->setOpenExternalLinks(true);
         qlbLinkSource->setText("<style> a {text-decoration: none} </style> <a href=\"https://github.com/rymcu/RYCOM\">--助手源代码--");// 无下划线
}

/***********************************************************
 * 窗口析造函数！
 ***********************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************
 * 串口接收数据延时
 * 1.串口收到数据后，先进入函数，重新启动延时定时器
 ***********************************************************/
void MainWindow::portRecvDataDelay()
{
    recvDelayTimer->stop();
    recvDelayTimer->start(10);//ms:延时时间按需选择
}

/***********************************************************
 * 状态栏时间，日期更新函数
 * 1.状态栏时间显示标签定时器超时函数，将当前时间设置到状态栏标签
 ***********************************************************/
void MainWindow::time_update()
{

    QDateTime current_time = QDateTime::currentDateTime();//获取时间
    QString timestr = current_time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示的格式

    currentTimeLabel->setText(timestr); //设置label的文本内容为时间
}

/***********************************************************
 * 状态栏标签显示计数值
 * 1.设置标签显示内容
 ***********************************************************/
void MainWindow::setNumOnLabel(QLabel *lbl, QString strS, long num)
{
    // 标签显示
    QString strN;
    strN.sprintf("%ld", num);
    QString str = strS + strN;
    lbl->setText(str);
}

/***********************************************************
 *串口接收槽函数
 * 1.接收数据，并显示在接收窗口
 * 2.显示过程中需要判断：
 *  2.1 是否需要16进制显示
 *  2.2 是否需要显示时间
 *  2.3 是否停止显示
 ***********************************************************/
void MainWindow::MyComRevSlot()
{
    QByteArray MyComRevBUff;//接收数据缓存
    QString StrTemp,StrTemp1,StrTimeDate;

    //停止接收延时定时器，读取串口接收到的数据，并格式化数据
    recvDelayTimer->stop();
    MyComRevBUff = MyCom.readAll();
    StrTemp = QString::fromLocal8Bit(MyComRevBUff);

    //接收流量统计，并显示到状态栏
    ComRevSum += MyComRevBUff.size();
    setNumOnLabel(qlbRevSum, "Rx: ", ComRevSum);

    //获取串口数据接收的系统时间，备后续使用
    curDateTime = QDateTime::currentDateTime();
    StrTimeDate = curDateTime.toString("[yyyy-MM-dd hh:mm:ss.zzz] ");

    //开始显示数据，显示模式包括：是否16进制，是否显示接收时间
    if(ui->checkBoxRevHex->checkState() == false)//正常文本显示
    {

        if(TimeDateDisp == true)//显示接收时间
        {
            StrTemp.prepend(StrTimeDate);//前面添加时间
            StrTemp.append("\r\n");//后面添加换行
        }
        if(StopDis == false)
         {
            ui->TextRev->insertPlainText(StrTemp);//显示数据
            ui->TextRev->moveCursor(QTextCursor::End);//光标移动到文本末尾
        }
    }
    else //16进制显示
    {
       StrTemp =  MyComRevBUff.toHex().toUpper();//转换为16进制数，并大写

       for(int i = 0; i<StrTemp.length (); i+=2)//整理字符串，即添加空格
       {
           StrTemp1 += StrTemp.mid (i,2);
           StrTemp1 += " ";
       }

       if(TimeDateDisp == true)//添加时间显示
       {
           StrTemp1.prepend(StrTimeDate);//前面添加时间
           StrTemp1.append("\r\n");//后面添加换行
       }
       if(StopDis == false)
       {
         ui->TextRev->insertPlainText(StrTemp1);//显示数据
         ui->TextRev->moveCursor(QTextCursor::End);//光标移动到文本末尾
       }
    }  

}

/***********************************************************
 *打开或关闭串口按钮槽函数
 * 1.从控件获取串口初始化信息包括：
 *  1.1 波特率、数据位、停止位、校验方式，端口号
 * 2.初始化串口
 * 3.打开串口
 *  3.1 打开成功，将按钮文字设置为“关闭串口”
 *  3.2 打开失败，弹出错误消息框
 *  3.3 处理相应控件
 * 4.关闭串口
 *  4.1 将按钮文字设置为“打开串口”
 *  4.2 处理相应控件
 ***********************************************************/
void MainWindow::on_pushButtonOpen_clicked()
{
    QSerialPort::BaudRate CombaudRate;
    QSerialPort::DataBits ComdataBits;
    QSerialPort::StopBits ComstopBits;
    QSerialPort::Parity   ComParity;

    //获取波特率
    if(ui->comboBoxComBaud->currentText()=="1200")
    {
        CombaudRate = QSerialPort::Baud1200;
    }
    else if(ui->comboBoxComBaud->currentText()=="2400")
    {
        CombaudRate = QSerialPort::Baud2400;
    }
    else if(ui->comboBoxComBaud->currentText()=="4800")
    {
        CombaudRate = QSerialPort::Baud4800;
    }
    else if(ui->comboBoxComBaud->currentText()=="9600")
    {
        CombaudRate = QSerialPort::Baud9600;
    }
    else if(ui->comboBoxComBaud->currentText()=="19200")
    {
        CombaudRate = QSerialPort::Baud19200;
    }
    else if(ui->comboBoxComBaud->currentText()=="38400")
    {
        CombaudRate = QSerialPort::Baud38400;
    }
    else if(ui->comboBoxComBaud->currentText()=="57600")
    {
        CombaudRate = QSerialPort::Baud57600;
    }
    else if(ui->comboBoxComBaud->currentText()=="115200")
    {
        CombaudRate = QSerialPort::Baud115200;
    }
    else
    {
        CombaudRate = QSerialPort::UnknownBaud;
    }

    //获取数据位
    if(ui->comboBoxData->currentText()=="5")
    {
        ComdataBits = QSerialPort::Data5;
    }
    else if(ui->comboBoxData->currentText()=="6")
    {
        ComdataBits = QSerialPort::Data6;
    }
    else if(ui->comboBoxData->currentText()=="7")
    {
        ComdataBits = QSerialPort::Data7;
    }
    else if(ui->comboBoxData->currentText()=="8")
    {
        ComdataBits = QSerialPort::Data8;
    }
    else
    {
        ComdataBits = QSerialPort::UnknownDataBits;
    }

    //获取停止位
    if(ui->comboBoxStop->currentText()=="1")
    {
        ComstopBits = QSerialPort::OneStop;
    }
    else if(ui->comboBoxStop->currentText()=="1.5")
    {
        ComstopBits = QSerialPort::OneAndHalfStop;
    }
    if(ui->comboBoxStop->currentText()=="2")
    {
        ComstopBits = QSerialPort::TwoStop;
    }
    else
    {
        ComstopBits = QSerialPort::UnknownStopBits;
    }

    //获取校验方式
    if(ui->comboBoxCheck->currentText()=="None")
    {
        ComParity = QSerialPort::NoParity;
    }
    else if(ui->comboBoxCheck->currentText()=="Even")
    {
        ComParity = QSerialPort::EvenParity;
    }
    else if(ui->comboBoxCheck->currentText()=="Odd")
    {
        ComParity = QSerialPort::OddParity;
    }
    else if(ui->comboBoxCheck->currentText()=="Space")
    {
        ComParity = QSerialPort::SpaceParity;
    }
    else if(ui->comboBoxCheck->currentText()=="Mark")
    {
        ComParity = QSerialPort::MarkParity;
    }
    else
    {
        ComParity = QSerialPort::UnknownParity;
    }

    //获取端口号
    QString spTxt = ui->comboBoxNo->currentText();
    spTxt = spTxt.section(':', 0, 0);//过滤掉:后面的信息，只留下COMx

    //初始化串口：设置端口号，波特率，数据位，停止位，校验方式等
    MyCom.setBaudRate(CombaudRate);
    MyCom.setDataBits(ComdataBits);
    MyCom.setStopBits(ComstopBits);
    MyCom.setParity(ComParity);
    MyCom.setPortName(spTxt);   //MyCom.setPortName(ui->comboBoxNo->currentText());

    //打开串口
    if(ui->pushButtonOpen->text() == "打开串口")
    {
        bool ComFlag;
        ComFlag = MyCom.open(QIODevice::ReadWrite);
        if(ComFlag == true)//串口打开成功
        {
            //串口下拉框设置为不可选
            ui->comboBoxCheck->setEnabled(false);
            ui->comboBoxComBaud->setEnabled(false);
            ui->comboBoxData->setEnabled(false);
            ui->comboBoxNo->setEnabled(false);
            ui->comboBoxStop->setEnabled(false);

            //使能相应按钮等
            ui->pushButtonSend->setEnabled(true);
            ui->checkBoxPeriodicSend->setEnabled(true);
            ui->checkBoxPeriodicMutiSend->setEnabled(true);
            ui->lineEditTime->setEnabled(true);
            ui->checkBoxAddNewShift->setEnabled(true);
            ui->checkBoxSendHex->setEnabled(true);

            ui->pushButtonOpen->setText("关闭串口");
        }
        else //打开失败，弹出窗口提示！
        {
           QMessageBox::critical(this, "错误提示", "串口打开失败，该端口可能被占用或不存在！\r\nLinux系统可能为当前用户无串口访问权限！");
        }
    }
    else
    {
       MyCom.close();

        ui->pushButtonOpen->setText("打开串口");
        ui->comboBoxCheck->setEnabled(true);
        ui->comboBoxComBaud->setEnabled(true);
        ui->comboBoxData->setEnabled(true);
        ui->comboBoxNo->setEnabled(true);
        ui->comboBoxStop->setEnabled(true);

        //使相应的按钮不可用
        ui->pushButtonSend->setEnabled(false);
        ui->checkBoxPeriodicSend->setChecked(false);//取消选中
        ui->checkBoxPeriodicSend->setEnabled(false);
        ui->checkBoxPeriodicMutiSend->setChecked(false);//取消选中
        ui->checkBoxPeriodicMutiSend->setEnabled(false);
        ui->lineEditTime->setEnabled(false);
        ui->checkBoxAddNewShift->setEnabled(false);
        ui->checkBoxSendHex->setEnabled(false);
    }
}

/***********************************************************
 *发送按钮槽函数
 * 1.读取发送窗口数据
 * 2.判断发送格式，并格式化数据
 *  2.1 十六进制发送，是否添加新行
 *  2.2 文本发送，是否添加新行
 * 3.发送串口数据
 * 4.统计发送流量，并显示在状态栏
 ***********************************************************/
void MainWindow::on_pushButtonSend_clicked()
{
   QByteArray ComSendData;
   QString SendTemp;
   int temp;

   //读取发送窗口数据
   SendTemp = ui->TextSend->toPlainText();

   //判断发送格式，并格式化数据
   if(ui->checkBoxSendHex->checkState() != false)//16进制发送
   {
       if(ui->checkBoxAddNewShift->checkState() != false)
       {
           SendTemp.append("0D0A");// 后面添加换行
       }
       ComSendData = QByteArray::fromHex(SendTemp.toUtf8()).data();//获取字符串
   }
   else //字符串形式发送
   {
       if(ui->checkBoxAddNewShift->checkState() != false)
       {
           SendTemp.append("\r\n");// 后面添加换行
       }
        ComSendData = SendTemp.toLocal8Bit().data();//获取字符串
   }
   //发送数据
   temp = MyCom.write(ComSendData);

   //统计发送流量，并显示在状态栏
   if(temp)
   {
       ComSendSum +=temp;//发送总数统计
       setNumOnLabel(qlbSendSum, "Tx: ", ComSendSum);// 状态栏显示计数值
   }
}

/***********************************************************
 *清空接收区按钮槽函数
 * 1.清空接收区
 * 2.清空流量统计
 ***********************************************************/
void MainWindow::on_pushButtonClearRev_clicked()
{
   ui->TextRev->clear();

   ComSendSum = 0;
   ComRevSum  = 0;

   setNumOnLabel(qlbSendSum, "Tx: ", ComSendSum);
   setNumOnLabel( qlbRevSum, "Rx: ", ComRevSum);
}

/***********************************************************
 *清空发送区按钮槽函数
 * 1.清空发送区
 * 2.清空流量统计
 ***********************************************************/
void MainWindow::on_pushButtonClearSend_clicked()
{
   ui->TextSend->clear();
   ComSendSum = 0;
   ComRevSum  = 0;

   setNumOnLabel(qlbSendSum, "Tx: ", ComSendSum);
   setNumOnLabel( qlbRevSum, "Rx: ", ComRevSum);
}

/***********************************************************
 *接收区停止显示槽函数
 * 1.设置停止接收标志位
 ***********************************************************/
void MainWindow::on_pushButtonStopRev_clicked()
{
    if(ui->pushButtonStopRev->text() == "停止显示")
    {
        ui->pushButtonStopRev->setText("继续显示");
        StopDis = true;
    }
    else
    {
        ui->pushButtonStopRev->setText("停止显示");
        StopDis = false;
    }

}

/***********************************************************
 *周期发送
 * 1.未选中状态下，关闭周期发送定时器，使能周期输入文本框
 * 2.选中状态,获取周期，并启动周期发送定时器，失能周期输入文本框
 * 3.选中状态下，取消多行周期发送状态，两者只能二选一
 ***********************************************************/
void MainWindow::on_checkBoxPeriodicSend_stateChanged(int arg1)
{
    if(arg1 == false)//未选中
    {
        PriecSendTimer->stop();//关闭定时器
        ui->lineEditTime->setEnabled(true);//使能周期输入文本框
    }
    else
    {
        ui->checkBoxPeriodicMutiSend->setChecked(false);//关闭多行循环发送
        PriecSendTimer->start(ui->lineEditTime->text().toInt());//启动周期发送定时器
        ui->lineEditTime->setEnabled(false);//失能周期输入文本框
    }
}

/***********************************************************
 *多行循环发送
 * 1.未选中状态下，关闭周期发送定时器，使能周期输入文本框
 * 2.选中状态,获取周期，并启动周期发送定时器，失能周期输入文本框
 * 3.选中状态下，取消周期发送状态，两者只能二选一（和周期发送共用一个定时器）
 ***********************************************************/
void MainWindow::on_checkBoxPeriodicMutiSend_stateChanged(int arg1)
{
    if(arg1 == false)//未选中
    {
        PriecSendTimer->stop();//关闭定时器
        ui->lineEditTime->setEnabled(true);//使能对话框编辑
    }
    else
    {
        LastSend = 0;//从第一行开始发送
        ui->checkBoxPeriodicSend->setChecked(false);
        PriecSendTimer->start(ui->lineEditTime->text().toInt());
        ui->lineEditTime->setEnabled(false);//关闭对话框编辑
    }
}

/***********************************************************
 *显示多行发送界面
 * 1.显示和隐藏多行发送界面
 ***********************************************************/
void MainWindow::on_radioButton_clicked()
{
    if(ui->radioButton->isChecked() == false)
    {
        ui->checkBoxPeriodicMutiSend->setChecked(false);
        ui->groupBoxMutiSend->hide();//隐藏多行发送界面
        //ui->groupBoxRev->setFixedWidth(541);
        //ui->TextRev->setFixedWidth(521);
        ui->groupBoxRev->setFixedWidth(541*myobjectRate);
        ui->TextRev->setFixedWidth(521*myobjectRate);
    }
    else
    {
        //ui->groupBoxRev->setFixedWidth(341);
        //ui->TextRev->setFixedWidth(321);
        ui->groupBoxRev->setFixedWidth(341*myobjectRate);
        ui->TextRev->setFixedWidth(321*myobjectRate);
        ui->groupBoxMutiSend->show();//显示多行发送界面
    }
}

/***********************************************************
 *设置是否显示接收时间的标志位
 * rt
 ***********************************************************/
void MainWindow::on_checkBoxReVTime_stateChanged(int arg1)
{
    if(arg1 == false)//未选中
    {
        TimeDateDisp = false;//接收串口数据时间显示标志位
    }
    else
    {
        TimeDateDisp = true;
    }
}
//读取文件,并显示在发送窗口

void MainWindow::on_pushButtonRdFile_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="打开一个文件"; //对话框标题
    QString filter="文本文件(*.txt);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
    openTextByIODevice(aFileName);

}

/***********************************************************
 *读文件函数
 * 读取文件，并将文件内容显示在发送框
 ***********************************************************/
bool MainWindow::openTextByIODevice(const QString &aFileName)
{
    QFile   aFile(aFileName);//用IODevice方式打开文本文件
    if (!aFile.exists()) //文件不存在
        return false;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QByteArray text = aFile.readAll();
    QString strText = byteArrayToUnicode(text);//编码格式转换，防止GBK中文乱码
    ui->TextSend->setPlainText(strText);
    aFile.close();
    return  true;
}
/***********************************************************
*编码格式转换
*防止GBK中文乱码
***********************************************************/
QString MainWindow::byteArrayToUnicode(const QByteArray &array)
{
    // state用于保存转换状态，它的成员invalidChars，可用来判断是否转换成功
    // 如果转换成功，则值为0，如果值大于0，则说明转换失败
    QTextCodec::ConverterState state;
    // 先尝试使用utf-8的方式把QByteArray转换成QString
    QString text = QTextCodec::codecForName("UTF-8")->toUnicode(array.constData(), array.size(), &state);
    // 如果转换时无效字符数量大于0，说明编码格式不对
    if (state.invalidChars > 0)
    {
        // 再尝试使用GBK的方式进行转换，一般就能转换正确(当然也可能是其它格式，但比较少见了)
        text = QTextCodec::codecForName("GBK")->toUnicode(array);
    }
    return text;
}

/***********************************************************
*保存文件按钮槽函数
*保存指定格式文件
***********************************************************/
void MainWindow::on_pushButtonSaveRev_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="另存为一个文件"; //对话框标题
    QString filter="文本文件(*.txt);;所有文件(*.*);;h文件(*.h);;c++文件(*.cpp)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
    saveTextByIODevice(aFileName);
}
/***********************************************************
*写文件函数
*执行文件保存工作
***********************************************************/
bool MainWindow::saveTextByIODevice(const QString &aFileName)
{ //用IODevice方式保存文本文件
    QFile aFile(aFileName);
    //aFile.setFileName(aFileName);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QString str=ui->TextRev->toPlainText();//整个内容作为字符串
    QByteArray  strBytes=str.toUtf8();//转换为字节数组

    aFile.write(strBytes,strBytes.length());  //写入文件
    aFile.close();

    return true;
}

/***********************************************************
*多行发送按钮1-10槽函数
*1.将发送的内容设置到发送框
*2.调用发送按钮槽函数，完成数据发送
***********************************************************/
void MainWindow::on_pushButtonMuti1_clicked()
{
    QString Strtemp = ui->lineEditMuti1->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti2_clicked()
{
    QString Strtemp = ui->lineEditMuti2->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti3_clicked()
{
    QString Strtemp = ui->lineEditMuti3->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti4_clicked()
{
    QString Strtemp = ui->lineEditMuti4->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti5_clicked()
{
    QString Strtemp = ui->lineEditMuti5->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti6_clicked()
{
    QString Strtemp = ui->lineEditMuti6->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti7_clicked()
{
    QString Strtemp = ui->lineEditMuti7->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti8_clicked()
{
    QString Strtemp = ui->lineEditMuti8->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti9_clicked()
{
    QString Strtemp = ui->lineEditMuti9->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

void MainWindow::on_pushButtonMuti10_clicked()
{
    QString Strtemp = ui->lineEditMuti10->text();
    ui->TextSend->clear();
    ui->TextSend->insertPlainText(Strtemp);
    ui->TextSend->moveCursor(QTextCursor::End);

    MainWindow::on_pushButtonSend_clicked();

}

/***********************************************************
*清除多行文本
*1.清空多行文本
***********************************************************/
void MainWindow::on_pushButtonMutiReset_clicked()
{
    ui->lineEditMuti1->clear();
    ui->lineEditMuti2->clear();
    ui->lineEditMuti3->clear();
    ui->lineEditMuti4->clear();
    ui->lineEditMuti5->clear();
    ui->lineEditMuti6->clear();
    ui->lineEditMuti7->clear();
    ui->lineEditMuti8->clear();
    ui->lineEditMuti9->clear();
    ui->lineEditMuti10->clear();
}

/***********************************************************
*多行周期发送预处理函数
*1.该函数直接与周期发送定时器超市函数关联
*2.判断是多行循环发送，还是周期发送
* 2.1 多行周期发送时，进行多行排序，并发送
* 2.2 周期发送时，直接调用调用发送按钮槽函数，完成数据发送
***********************************************************/
void MainWindow::Pre_on_pushButtonSend_clicked()
{
    int EditLineNo=0;
    //static int LastSend = 0;

   if(ui->checkBoxPeriodicMutiSend->isChecked() == true)
    {
       if(LastSend >=Get_checkBoxMuti_State()) LastSend = 0;

       //Get_checkBoxMuti_State();//获取多行周期发送的选中状态

        for(int i = LastSend;i < 10;i++)
        {
           if(MutiState[i] == true)
           {
                EditLineNo = i+1;
                LastSend = i+1;
                break;
            }
        }
        SendDataByNoOfEditLineNo(EditLineNo);//根据状态,发送指定行的数据
    }
  else
   {
         on_pushButtonSend_clicked();
   }

}

/***********************************************************
*获取多行周期发送的选中状态
*1.将10行的选中状态存储在MutiState[10]数组中
***********************************************************/
int MainWindow::Get_checkBoxMuti_State()
{
    int temp;

   if(ui->checkBoxMuti_1->isChecked() == true)
   {
       MutiState[0]= true;
   }
   else
   {
       MutiState[0]= false;
   }
   if(ui->checkBoxMuti_2->isChecked() == true)
   {
       MutiState[1]= true;
   }
   else
   {
       MutiState[1]= false;
   }
   if(ui->checkBoxMuti_3->isChecked() == true)
   {
       MutiState[2]= true;
   }
   else
   {
       MutiState[2]= false;
   }
   if(ui->checkBoxMuti_4->isChecked() == true)
   {
       MutiState[3]= true;
   }
   else
   {
       MutiState[3]= false;
   }
   if(ui->checkBoxMuti_5->isChecked() == true)
   {
       MutiState[4]= true;
   }
   else
   {
       MutiState[4]= false;
   }
   if(ui->checkBoxMuti_6->isChecked() == true)
   {
       MutiState[5]= true;
   }
   else
   {
       MutiState[5]= false;
   }
   if(ui->checkBoxMuti_7->isChecked() == true)
   {
       MutiState[6]= true;
   }
   else
   {
       MutiState[6]= false;
   }
   if(ui->checkBoxMuti_8->isChecked() == true)
   {
       MutiState[7]= true;
   }
   else
   {
       MutiState[7]= false;
   }
   if(ui->checkBoxMuti_9->isChecked() == true)
   {
       MutiState[8]= true;
   }
   else
   {
       MutiState[8]= false;
   }
   if(ui->checkBoxMuti_10->isChecked() == true)
   {
       MutiState[9]= true;
   }
   else
   {
       MutiState[9]= false;
   }
    for(int i = 9;i>0;i--)
    {
       if( MutiState[i]==true)
       {
           temp = i+1;
           break;
       }
       else
           temp = 0;
    }
    return temp;
}

/***********************************************************
*根据状态,发送指定行的数据
*
***********************************************************/
void MainWindow::SendDataByNoOfEditLineNo(int EditLineNo)
{
    switch (EditLineNo) {
    case 1:
        on_pushButtonMuti1_clicked();
        break;
    case 2:
        on_pushButtonMuti2_clicked();
        break;
    case 3:
        on_pushButtonMuti3_clicked();
        break;
    case 4:
        on_pushButtonMuti4_clicked();
        break;
    case 5:
        on_pushButtonMuti5_clicked();
        break;
    case 6:
        on_pushButtonMuti6_clicked();
        break;
    case 7:
        on_pushButtonMuti7_clicked();
        break;
    case 8:
        on_pushButtonMuti8_clicked();
        break;
    case 9:
        on_pushButtonMuti9_clicked();
        break;
    case 10:
        on_pushButtonMuti10_clicked();
        break;
    default:
        break;
    }
}

/***********************************************************
*刷新dpi，获取当前屏幕状态，并伸缩所用控件的大小
*只在窗口构造函数中调用，因此，改变屏幕分辨率需要重新启动软件
***********************************************************/
void MainWindow::refreshDPI()
{
    //计算dpi
    QList<QScreen*> screens = QApplication::screens();
    QScreen* screen = screens[0];
    qreal dpi = screen->logicalDotsPerInch();

    //计算dpi对应的缩放比例
    double objectRate = dpi/96.0;
    myobjectRate=objectRate;
    changeObjectSize(*this, objectRate);
    resize(width()*objectRate,height()*objectRate);
}
/***********************************************************
* 根据屏幕清空调整控件大小
***********************************************************/
void MainWindow::changeObjectSize(const QObject &o, double objectRate)
{
    for (int i=0; i<o.children().size(); ++i) {
        QWidget *pWidget = qobject_cast<QWidget *>(o.children().at(i));
        if (pWidget != nullptr) {
            pWidget->setGeometry(pWidget->x()*objectRate,pWidget->y()*objectRate,
                                 pWidget->width()*objectRate, pWidget->height()*objectRate);
            changeObjectSize(*(o.children().at(i)),objectRate);
        }
    }
}
