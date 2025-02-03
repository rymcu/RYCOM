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
        this->setFixedSize(729*myobjectRate,564*myobjectRate);//根据屏幕分辨率不一样固定主窗口的大小


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

        //创建文件监控定时器
         FileChanged = new QTimer;
         connect(FileChanged,&QTimer::timeout,this,[=](){timer_FileChanged();});
         FileChanged->start(1000);

        //串口接收函数关联
        connect(&MyCom,SIGNAL(readyRead()),this,SLOT(portRecvDataDelay()));//串口数据接收过程：系统收到串口数据后，进入接收延时函数portRecvDataDelay(),
                                                                           //函数中停止定时器，再启动定时器，定时时间到了再进入接收处理函数MyComRevSlot()
                                                                           //因此，只有收到数据后，在连续的定时时间内都没有数据到了，才会进行数据处理，否则
                                                                           //一直在接收数据，这是为了解决收中文数据时，分段接收显示乱码问题。
        //增加波特率自定义输入
        connect(ui->comboBoxComBaud,SIGNAL(currentIndexChanged(int)),this,SLOT(SLOT_baudIndexChanged(int)));
        //connect(ui->comboBoxComBaud,SIGNAL( currentTextChanged(int)),this,SLOT(SLOT_baudChanged(QString)));
        //增加文件更改监控
        //my_watcher = new QFileSystemWatcher(this);//文件监控对象
        //my_watcher->addPath("C:/Users/Hugh/Desktop/temp/Template.hex");
        //connect(my_watcher,SIGNAL(fileChanged(QString)),this,SLOT(MyFileUpdated()));

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
         qlbLinkSource->setText("<style> a {text-decoration: none} </style> <a href=\"https://github.com/rymcu/RYCOM\">--助手源代码V2.6.0--");// 无下划线
         //隐藏进度条
         ui->progressBar->setVisible(false);
         //串口指示灯设置为只读控件，屏蔽鼠标点击事件
         //ui->radioButton_led->setAttribute(Qt::WA_TransparentForMouseEvents,QIODevice::ReadOnly);
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
    if(ISisping==2)return;//ESP32下载时，此处不处理数据，在下载流程中读取数据
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
    //QByteArray MyComRevBUff;//接收数据缓存
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

    //ISP期间不显示接收时间
    //if(ISisping) return;

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

    //串口接收到数据标识
    //ISMyComRevData = 1;
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
     else if(ui->comboBoxComBaud->currentText()=="230400")//添加高波特率支持，更改qserialport.h中enum BaudRate.
     {
         CombaudRate = QSerialPort::Baud230400;
     }
     else if(ui->comboBoxComBaud->currentText()=="345600")
     {
         CombaudRate = QSerialPort::Baud345600;
     }
     else if(ui->comboBoxComBaud->currentText()=="460800")
     {
         CombaudRate = QSerialPort::Baud460800;
     }
     else if(ui->comboBoxComBaud->currentText()=="576000")
     {
         CombaudRate = QSerialPort::Baud576000;
     }
     else if(ui->comboBoxComBaud->currentText()=="921600")
     {
         CombaudRate = QSerialPort::Baud921600;
     }
     else if(ui->comboBoxComBaud->currentText()=="1382400")
     {
         CombaudRate = QSerialPort::Baud1382400;
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

    //将自定义串口文本转换为int
    int my_baud = CombaudRate;
    if(14 ==ui->comboBoxComBaud->currentIndex())
    {
       QString str=ui->comboBoxComBaud->currentText();
        my_baud = str.toInt();
    }

     ESP32_loader_daud = my_baud;
    //设置STM32，ESP32下载时的校验位
    if(ISisping == 1) ComParity = QSerialPort::EvenParity;//STM32下载校验位必须为EVEN
    if(ISisping == 2)
    {
        ComParity = QSerialPort::NoParity;//ESP32下载校验位必须为NONE
         my_baud = QSerialPort::Baud115200;//同步时必须为115200,如果正在同步时序中，强制设置成115200
    }
    //初始化串口：设置端口号，波特率，数据位，停止位，校验方式等
    MyCom.setBaudRate(my_baud);
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
            ui->pushButton_RYISP->setEnabled(true);//stm32
            ui->pushButton_ESP32ISP->setEnabled(true);//esp32
            ui->pushButton_Ymodem->setEnabled(true);

            ui->pushButtonOpen->setText("关闭串口");

            ui->radioButton_led->setChecked(true);//点亮串口LED

            ISisping = 0;//清空ISP表示，接收区正常显示数据
        }
        else //打开失败，弹出窗口提示！
        {
           QMessageBox::critical(this, "错误提示", "串口打开失败!\r\n该端口可能被占用或不存在或波特率设置错误！\r\n也可能访问串口权限关闭！");
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
        ui->pushButton_RYISP->setEnabled(false);//stm32
        ui->pushButton_Ymodem->setEnabled(false);
        ui->pushButton_ESP32ISP->setEnabled(false);//esp32

        ui->radioButton_led->setChecked(false);//关闭串口LED
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
  // char *buff_Hex = NULL;
    char buff_Hex[65535] ={0};
   int len_Hex = 0;

   //读取发送窗口数据
   SendTemp = ui->TextSend->toPlainText();


   //判断发送格式，并格式化数据
   if(ui->checkBoxSendHex->checkState() != false)//16进制发送
   {
       SendTemp = SendTemp.toUtf8();

       SendTemp.remove(QChar(' '),Qt::CaseInsensitive);
       SendTemp.remove(QChar('\n'),Qt::CaseInsensitive);
       SendTemp.remove(QChar('\r'),Qt::CaseInsensitive);



       if(ui->checkBoxAddNewShift->checkState() != false)
       {
           SendTemp.append("0D0A");// 后面添加换行
       }

       //SendTemp.toUpper();

       len_Hex = SendTemp.size();

       if(len_Hex%2==0)
       {
           for(int i=0;i<len_Hex/2;i++)
           {
              unsigned char temp_A,temp_B,A,B;

              temp_A = (SendTemp.at(2*i)).unicode();
              if((temp_A <= '9') &&(temp_A >= '0'))
              {
                  A = temp_A-'0';
              }
              else if((temp_A <= 'F')&&(temp_A >= 'A'))
              {
                  A = temp_A-('A'-10);
              }
              else
              {
                  A = temp_A-('a'-10);
              }

              temp_B =  (SendTemp.at(2*i+1)).unicode();

              if((temp_B <= '9') &&(temp_B >= '0'))
              {
                  B = temp_B-'0';
              }
              else if((temp_B <= 'F')&&(temp_B >= 'A'))
              {
                  B = temp_B-('A'-10);
              }
              else
              {
                  B = temp_B-('a'-10);
              }

              buff_Hex[i]=A*16 + B;
            }
           //ComSendData = SendTemp.toLocal8Bit().data();//获取字符串

           //发送数据
           //temp = MyCom.write(ComSendData);
         //发送数据
         //buff_Hex[0] = len_Hex;
         temp = MyCom.write(buff_Hex,len_Hex/2);
       }
       else
       {
           temp=0;
       }

   }
   else //字符串形式发送
   {
       if(ui->checkBoxAddNewShift->checkState() != false)
       {
           SendTemp.append("\r\n");// 后面添加换行
       }
        ComSendData = SendTemp.toLocal8Bit().data();//获取字符串

        //发送数据
        temp = MyCom.write(ComSendData);
   }

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
    if (aFileName.isEmpty()) return;
/*
    QByteArray ba = aFileName.toLatin1();
    char *c_str = ba.data();

    char buffer_hex[600];
    FILE *src_file;
    src_file = fopen(c_str, "r");		//以文本的形式打开一个hex文件
    fseek(src_file, 0, SEEK_SET);           //定位到开头，准备开始读取数据
    fscanf(src_file, "%s\r\n", buffer_hex);
    QString str(buffer_hex);
    ui->TextRev->insertPlainText(str);//显示数据

    unsigned int Low,High,addr_temp = 0;
    if(buffer_hex[2] == '2')
    {

        for(char i=0;i<4;i++)
        {
            Low = buffer_hex[6-i]-'0';
            Low = Low<<(4*i);

           High = buffer_hex[12-i]-'0';
           High = High<<(4*i+16);

            addr_temp = addr_temp + Low + High;
        }
    }

    QString str1;
    str1= QString::number(addr_temp,10);
    ui->TextRev->insertPlainText(str1);
    */

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
    if (!aFile.open(QIODevice::ReadOnly))//if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QByteArray text = aFile.readAll();
    QString strText = text.toHex().toUpper();

    //QString strText = byteArrayToUnicode(text);//编码格式转换，防止GBK中文乱码
    //ui->TextSend->setPlainText(strText);
    MyCom.write(text);
    QString temptext = "send ok!";
    ui->TextSend->setPlainText(temptext);
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
    double objectRate = dpi/96.0;//72
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



/***********************************************************
* char转16进制
***********************************************************/
QString CharToHex(char *data,char num)
{
     QString  StrTemp,StrTemp1;
     QByteArray  temptset;
    for(int i=0;i<num;i++) temptset[i]=data[i];

     StrTemp = temptset.toHex().toUpper();//转换为16进制数，并大写
     for(int i = 0; i<StrTemp.length (); i+=2)//整理字符串，即添加空格
     {
         StrTemp1 += StrTemp.mid (i,2);
         StrTemp1 += " ";
     }

     return StrTemp1;
}
/***********************************************************
* char转16进制
***********************************************************/
void MainWindow::ResumeFormISP(void)
{
    ui->groupBoxRev->setTitle("接收区");
    ISisping = 0;
    ui->progressBar->setVisible(false);//隐藏进度条
    on_pushButtonOpen_clicked();//重启串口，复位CH343P状态，保证能正常下载
    on_pushButtonOpen_clicked();
}
void MainWindow::ResumeFormESP32ISP(void)
{
    ui->groupBoxRev->setTitle("接收区");
    ISisping = 0;
    //ui->progressBar->setVisible(false);//隐藏进度条
    on_pushButton_ESP32ISP_clicked();//恢复状态

    on_pushButtonOpen_clicked();//重启串口，复位CH343P状态，保证能正常下载
    on_pushButtonOpen_clicked();
}
/***********************************************************
* 启动串口ISP
* step1 串口设置为Even校验，并打开后，启动串口ISP按钮有效
* step2 单机按钮进入ISP流程；
* step3 清空接收和显示文本框，并判断是否正在下载标志ISisping,解决了多
*       次点击，重复进入槽函数执行的情况；
* step4 读取.hex或.bin文件，读取成功返回1；
* step5 每隔300ms发送一次同步信号0x7F,总共循环20次，循环内收到ACK后
*       进入下一步，无应答则退出ISP流程；
* step6 读取设备ID并显示；
* step7 读取bootloader版本号显示，并读取支持的CMD命令
* step8 全片擦除内存；
* step9 使能进度条，开始显示数据并更新进度条
* step10 下载完完成，隐藏进度条，并跳转至程序开始处执行。
* step11 结束。
***********************************************************/
void MainWindow::on_pushButton_RYISP_clicked()
{
     char error;

     //设置进入ISP标志
     if(ISisping != 0) return;//ISP过程中再次点击按钮，直接返回
     ISisping = 1;
     on_pushButtonOpen_clicked();//重启串口，复位CH343P状态，保证能正常下载
     on_pushButtonOpen_clicked();
     ISisping = 1;//重新打开会将ISisping清零，这里再次赋值为1

     //清除屏幕,设置标题,清空串口接收缓冲区
     ui->groupBoxRev->setTitle("STM32 ISP log");
     ui->TextRev->clear();
     ui->TextRev->clear();
     MyComRevBUff.clear();

     delay_msec(1000);//等待文件更新，Keil编译时，先删除.hex，再新建，等待1s,等文件生成好后再打开。
     //读取.hex或.bin内容
     error = OpenMyFile();
     if(!error)
     {
         QMessageBox::critical(this, "提示", "文件打开失败!");
          ResumeFormISP();
         return;
     }

    //循环握手20次，失败提示，成功继续
    unsigned char StartTimes = 14;//20;

    ui->TextRev->insertPlainText("\r\n------------------------启动Bootloader-----------------------\r\n");
    ui->TextRev->insertPlainText("启动串口下载ISPing...");//显示数据
    //使开发板进入串口ISP模式

    SetRTS(true);
    SetDTR(true);
    SetRTS(false);
    delay_msec(200);
    SetDTR(false);

    SetRTS(true);
    SetDTR(true);
    SetRTS(false);
    delay_msec(200);
    SetDTR(false);

    MyComRevBUff.clear();//再次清空，防止延时期间收到数据干扰


    while(StartTimes--)
    {
       if(StartISP()) break;
       else   ui->TextRev->insertPlainText("...");//显示数据
       delay_msec(300);
       if(StartTimes==1)
       {
         //QMessageBox::critical(this, "提示", "Activating device: Fail.\r\n Please, verify the boot mode configuration and check the serial port configuration. \r\nReset your device then try again...");
         QMessageBox::critical(this, "提示", "进入bootloader失败.\r\n 请确认boot和串口设置. \r\n重启设备...");
         ui->TextRev->insertPlainText("\r\n进入bootloader：失败!");//显示数据
         ResumeFormISP();
         return;
       }
    }
    //ui->TextRev->insertPlainText("\r\nActivating device: OK!\r\n\r\n");
    ui->TextRev->insertPlainText("\r\n进入Bootloader: 成功!\r\n\r\n");

    //获取Device ID
    error = GetID(&DeviceInfo);//获取从机ID

    if(error)
    {
        ui->TextRev->insertPlainText("\r\n--------------------------获取设备信息------------------------\r\n");
        ui->TextRev->insertPlainText("设备ID:0x");//显示数据，//ui->TextRev->insertPlainText("Device ID:0x");//显示数据
        ui->TextRev->insertPlainText(CharToHex(DeviceInfo.PID,2));//显示数据
        ui->TextRev->insertPlainText("\r\n");//显示数据
        ui->TextRev->moveCursor(QTextCursor::End);//光标移动到文本末尾
    }
    else
    {
        //QMessageBox::critical(this, "提示", "Get device ID fail!\r\nIt's COM Parity Even?");
        QMessageBox::critical(this, "提示", "获取设备ID 失败!\r\n确认串口校验是否为Even?");
        ResumeFormISP();
        return;
    }

    //获取从机指令
    error = GetCMD(&DeviceInfo);
    if(error)
    {
        unsigned char temp[3];
        temp[0] = (unsigned char)DeviceInfo.bootloaderversion /16 + '0';
        temp[1] = '.';
        temp[2] = (unsigned char)DeviceInfo.bootloaderversion %16 + '0';
        QString str;
        str[0] = temp[0];
        str[1] = temp[1];
        str[2] = temp[2];

        ui->TextRev->insertPlainText("当前Bootloader版本: V");//显示数据//ui->TextRev->insertPlainText("BootLoader Version: V");//显示数据

        ui->TextRev->insertPlainText(str);//显示数据
        ui->TextRev->insertPlainText("\r\n");//显示数据
        //ui->TextRev->insertPlainText("CMD: ");//显示数据
        //ui->TextRev->insertPlainText(CharToHex(DeviceInfo.cmd,DeviceInfo.cmd_count));//显示数据
        ui->TextRev->insertPlainText("\r\n");//显示数据
        ui->TextRev->moveCursor(QTextCursor::End);//光标移动到文本末尾
    }
    else
    {
      //QMessageBox::critical(this, "提示", "Get device CMD fail!");
        QMessageBox::critical(this, "提示", "获取设备CMD失败!");
      ResumeFormISP();
      return;
    }
   //擦除从机Flash
    if(ui->radioButton_EraseALL->isChecked())
    {
       ui->TextRev->insertPlainText("\r\n-------------------------开始擦除FLASH------------------------\r\n");
       ui->TextRev->insertPlainText("正在全片擦除，时间较长，请您耐心等候....\r\n");//显示数据，   //ui->TextRev->insertPlainText("Erasing....\r\n");//显示数据
       on_pushButton_EraseAll_clicked();
    }
    else
    {
      ui->TextRev->insertPlainText("不擦除内部FLASH....\r\n");
    }

/*
   //read flash
   unsigned char dataflash[4];
   //ReadMem(dataflash,0x1FFFF7E0,3);
   QString str;
   str[0] = dataflash[3];
   str[1] = dataflash[2];
   str[2] = dataflash[1];
   str[3] = dataflash[0];

ui->TextRev->insertPlainText(CharToHex((char *)dataflash,4));//显示数据
*/

   //开始下载程序
    ui->progressBar->setVisible(true);//显示进度条
   on_pushButton_WriteBin_clicked();
   //状态恢复
   ResumeFormISP();
}
/***********************************************************
* 打开一个.bin或.hex格式文件，并将文件路径设置到界面文本框中
***********************************************************/
void MainWindow::GetFileName()
{
    //写文件
     QString curPath=QDir::currentPath();//获取系统当前目录
     QString dlgTitle="打开一个文件"; //对话框标题
     //QString filter="(*.hex);;(*.bin);;所有文件(*.*)"; //文件过滤器
     QString filter="(*.hex *.bin)"; //文件过滤器
     QString aFileName=QFileDialog::getOpenFileName(NULL,dlgTitle,curPath,filter);

    if (!aFileName.isEmpty())
    {
        ui->lineEditMuti_FileName->setText(aFileName);
        QFileInfo fileinfo(ui->lineEditMuti_FileName->text());//获取当前文件的最新修改时间
        lastmodified = fileinfo.lastModified();

        //if(ui->checkBoxAutoDownload->checkState() != false)//判断是否使能自动下载更能
        //{
            //if(!my_watcher->files().isEmpty()) my_watcher->removePath(ui->lineEditMuti_FileName->text());
            //my_watcher->addPath(ui->lineEditMuti_FileName->text());//添加文件监控路径
        //}
    }
    else
    {
        QMessageBox::critical(this, "提示", "文件为空！");
    }
    //测试Qfileinfo

}
/***********************************************************
* 打开文件，并读取数据及程序首地址
*
* step1 从界面文本框中读取文件路径
* step2 .hex文件则解析并将数据存储到全局变量text中,同时返回程序首
*       地址到ISBeginAddr
* step3 .bin文件则将数据读入text变量中，并从文本框中读取程序首地址
*       到ISBeginAddr
***********************************************************/
char MainWindow:: OpenMyFile()
{
     QString aFileName;
    text.clear();
    aFileName = ui->lineEditMuti_FileName->text();

     if (aFileName.isEmpty()) return 0;

     if(aFileName.contains(".hex"))
     {
         QByteArray ba = aFileName.toLatin1();
         char *c_str = ba.data();
         text.clear();

         //RESULT_STATUS res = HexFile2BinFile(c_str, "/") ;
         //if(res != RES_OK) return;
         //unsigned int addr;
         if(HexFile2BinFile(c_str, "/",&ISPBeginAddr) == RES_OK)
             //ISPBeginAddr = GetProgramBeginAddr(); /////////////////////////////////////////////
             return 0;
     }

    else if(aFileName.contains(".bin"))
     {

         QFile   aFile(aFileName);//用IODevice方式打开文本文件
         if (!aFile.exists()) //文件不存在
          return 0;
         if (!aFile.open(QIODevice::ReadOnly))
          return 0;
          text = aFile.readAll();
         aFile.close();
         //获取程序首地址
         ISPBeginAddr = GetProgramBeginAddr();//bin文件从界面获得首地址
     }

     if(text.isNull())
     {
         //QMessageBox::critical(this, "提示", "Open a .bin or .hex file!");
         return 0;

     }
     return 1;
}
/***********************************************************
*
*  擦除全片
*
***********************************************************/
void MainWindow::on_pushButton_EraseAll_clicked()
{
   char error;
   if(DeviceInfo.bootloaderversion < 0x30)
   {
       error = EraseAll();
   }
   else
   {
    error = ExtendedEraseAll();
   }


    if(error)  ui->TextRev->insertPlainText("全片擦除成功!\r\n");//ui->TextRev->insertPlainText("EraseAll:OK!\r\n");

    else       ui->TextRev->insertPlainText("擦除失败!\r\n");//ui->TextRev->insertPlainText("EraseAll:Fail!\r\n");
}
/***********************************************************
* 写入bin文件
***********************************************************/
void MainWindow::on_pushButton_WriteBin_clicked()
{
     if(text.isNull())
     {
         QMessageBox::critical(this, "提示", "请选择.bin或.Hex格式文件!");//QMessageBox::critical(this, "提示", "Open a .bin file!");
         ui->TextRev->insertPlainText("请选择.bin或.Hex格式文件!\r\n");//ui->TextRev->insertPlainText("choose a bin file!\r\n");
         return;

     }

     unsigned int len=text.size();
     char *TextToChar;
     TextToChar = text.data();

     //获取程序首地址
     int addr = ISPBeginAddr;
     //addr = GetProgramBeginAddr();
     unsigned char error;

         QString str;
         str= QString::number(addr,16); str = str.toUpper();
           ui->TextRev->insertPlainText("\r\n-------------------------开始程序下载-------------------------\r\n");
         ui->TextRev->insertPlainText("程序下载开始地址：0x");//ui->TextRev->insertPlainText("\r\nUpload begin address 0x");

        ui->TextRev->insertPlainText(str);


   ui->TextRev->insertPlainText("\r\n正在下载......");//ui->TextRev->insertPlainText("\r\nUploading...");
   ui->progressBar->setMaximum(len);
   for(unsigned int i=0;i<len/256;i++)
     {
        error = WriteMem((unsigned char*)(TextToChar + 256*i), addr + 256*i, 255);
        if(error)
        {
           //ui->TextRev->insertPlainText(".");
           ui->progressBar->setValue((i+1)*256);

        }
        else
        {
            QMessageBox::critical(this, "提示", "下载失败!");//QMessageBox::critical(this, "提示", "UPloading Fail!");
            return;
        }
     }
   if(len%256)
   {
       error = WriteMem((unsigned char*)(TextToChar + 256*(len/256)), addr+256*(len/256), (len%256)-1);
       if(error)
       {
          //ui->TextRev->insertPlainText(".");
          ui->progressBar->setValue(len);
       }
       else
       {
           QMessageBox::critical(this, "提示", "下载失败!");//QMessageBox::critical(this, "提示", "UPloading Fail!");
           return;
       }
       ui->progressBar->setValue(0);//最后清零，防止下次下载是从最大开始
   }
    ui->TextRev->insertPlainText("\r\n\r\n------------------------下载成功!enjoy!-----------------------");
    ui->TextRev->insertPlainText("\r\n\r\n-----------RYMCU嵌入式知识学习交流平台(rymcu.com)-------------\r\n");//ui->TextRev->insertPlainText("\r\n\r\n------succeed!------\r\n");
   error = CMDGo(addr);

   QString temp = QString::number(addr,16);
    temp = temp.toUpper();
   if(error)
   {
      ui->TextRev->insertPlainText("程序开始运行：0x");//ui->TextRev->insertPlainText("Program run from 0x");
      ui->TextRev->insertPlainText(temp);
   }
   else      ui->TextRev->insertPlainText("\r\n\r\n 程序运行失败!\r\n");//ui->TextRev->insertPlainText("\r\n\r\n Program run Fail!\r\n");
}
/***********************************************************
*按钮功能：
*  打开文件，并获取文件路径
***********************************************************/

void MainWindow::on_pushButton_OpenBin_clicked()
{
     GetFileName();
}

/***********************************************************
*
*   从文本框中获取编程首地址
*
***********************************************************/
int MainWindow::GetProgramBeginAddr()
{
    QString Strtemp = ui->lineEditMuti_binaddr->text();
    QByteArray StrByte =Strtemp.toUtf8();
    unsigned int len1 = StrByte.size();
    if(len1 != 10)
    {
        QMessageBox::critical(this, "提示", "Wrong address!");
        ui->TextRev->insertPlainText("please input the right  8Byte address!\r\n");
        return 0;
    }
    unsigned int tempchar[8];
     for(int i=0;i<8;i++)
    {
        tempchar[i] = StrByte.at(9-i);
    }
    for(int i=0;i<8;i++)
    {
        if(StrByte.at(9-i)>='0' && StrByte.at(9-i)<='9')
        {
            tempchar[i] = StrByte.at(9-i) -'0';
        }
        else if(StrByte.at(9-i)>='a' && StrByte.at(9-i)<='f')
        {
            tempchar[i] = StrByte.at(9-i) -'a'+ 10;
        }
        else if(StrByte.at(9-i)>='A' && StrByte.at(9-i)<='F')
        {
            tempchar[i] = StrByte.at(9-i) -'A'+ 10;
        }
        else
            return 0;
    }
    int addr = 0;
    for(int i=0;i<8;i++)
    {
        addr =addr + (tempchar[i]<<(4*i));
    }
    return addr;
}
//Ymodem支持待开发
void MainWindow::on_pushButton_Ymodem_clicked()
{
    //QMessageBox::critical(this, "提示", "Ymodem传输文件开发中,静候佳音!");
    //设置进入ISP标志
    if(ISisping == 1) return;//ISP过程中再次点击按钮，直接返回
    ISisping = 1;
    //等待
     MyComRevBUff.clear();
   if(0 != ry_ymodem.TxStart()) {ui->TextRev->insertPlainText("等待超时！\r\n");ISisping = 0; return;}

   //发送第一包
    if(0 != ry_ymodem.TxSoh(ui->lineEditMuti_FileName->text())) {ui->TextRev->insertPlainText("文件打开失败！\r\n");ISisping = 0;return;}
    PrintFileinfo();
  //发送数据包
    //隐藏进度条
    ui->progressBar->setVisible(true);
     ui->progressBar->setMaximum(ry_ymodem.FileSize/YMODEM_PACKET_1K_SIZE);
     //ui->TextRev->insertPlainText("开始发送文件!\r\n");
    for(int32_t i=0;i<=ry_ymodem.FileSize/YMODEM_PACKET_1K_SIZE;i++)
    {
         if(0 != ry_ymodem.TxData(i)) {ui->TextRev->insertPlainText("数据包错误!\r\n");ui->progressBar->setValue(0);ISisping = 0;return;}
        ui->progressBar->setValue(ry_ymodem.YmodemProgress);
        ui->TextRev->insertPlainText(".");
    }
    //隐藏进度条
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);

   //发送结束
   if(0 != ry_ymodem.TxEot()) {ui->TextRev->insertPlainText("EOT错误！\r\n");ISisping = 0;return;}
   //发送结束
   if(0 != ry_ymodem.TxEnd()) {ui->TextRev->insertPlainText("END错误！\r\n");ISisping = 0;return;}
    ui->TextRev->insertPlainText("\r\n100% Succeed! 0 Errors!");
    ISisping = 0;
    return;
}
void MainWindow::PrintFileinfo(void)
{
//QFileInfo info
    QString FileName = ry_ymodem.info.fileName();//获得文件名字
    float size = ry_ymodem.info.size()/1024.0;
    QString FileSize = QString::number(size,'f',2);//文件大小
    ui->TextRev->insertPlainText("Starting ymodem transfer.\r\nTransferring "+ FileName + "(" + FileSize +"KB)\r\n downloading");

}
/***********************************************************
* 当选择自定义时，将baud控件更改为文本输入框
***********************************************************/
void MainWindow::SLOT_baudIndexChanged(int baudIndex)//波特率选择为自定义时
{
    if(14 == baudIndex)
    {
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setFocus();
         QTimer::singleShot(0,lineEdit,lineEdit->selectAll);
        ui->comboBoxComBaud->setLineEdit(lineEdit);
    }
    else
    {
        ui->comboBoxComBaud->setEditable(false);
    }
}

/***********************************************************
 * 文件修改定时器
 * 1.
 ***********************************************************/
void MainWindow::timer_FileChanged()
{
    QFileInfo fileinfo(ui->lineEditMuti_FileName->text());//获取当前文件的最新修改时间
    QDateTime nowModified = fileinfo.lastModified();
    if(nowModified !=lastmodified)
    {
        lastmodified = nowModified; //ui->TextRev->insertPlainText("文件被修改了");
        if(ui->checkBoxAutoDownload->checkState() != false)//判断是否使能自动下载更能
        {
           on_pushButton_RYISP_clicked();//ui->TextSend->insertPlainText("\r\nFileUpdated");
        }
    }
}


void MainWindow::on_pushButton_ESP32ISP_clicked()
{
    static uint8_t flag = 0;
    ui->progressBar_BOOT_Combine->setVisible(false);//隐藏进度条
    ui->progressBar_PART->setVisible(false);
    ui->progressBar_APP->setVisible(false);
    flag++;
    if(flag%2)
    {
        ui->groupBox_esp32->move(190*myobjectRate,395*myobjectRate);
        ui->groupBoxSend->hide();
    }
    else
    {
        ui->groupBox_esp32->move(190*myobjectRate,596*myobjectRate);
        ui->groupBoxSend->show();
    }
}

void MainWindow::on_radioButton_combine_clicked()
{
    static QString temp;
    if(ui->radioButton_combine->isChecked() == true)
    {
        temp = ui->label_BOOT_Combine->text();

       ui->pushButton_Open_BOOT_Combine->setText("Combine");
       ui->lineEdit_BOOT_Combine->setPlaceholderText("选择合并后的.bin"); // 设置占位符文本
       ui->label_BOOT_Combine->setText("@ 0x0");

        ui->pushButton_Open_PART->hide();
        ui->lineEdit_PART->hide();
        ui->label_PART->hide();
        ui->pushButton_Open_APP->hide();
        ui->lineEdit_APP->hide();
        ui->label_APP->hide();


    }
    else
    {
        ui->pushButton_Open_BOOT_Combine->setText("打开BOOT");
        ui->lineEdit_BOOT_Combine->setPlaceholderText("选择bootlaoder.bin"); // 设置占位符文本
        ui->label_BOOT_Combine->setText(temp);

        ui->pushButton_Open_PART->show();
        ui->lineEdit_PART->show();
        ui->label_PART->show();
        ui->pushButton_Open_APP->show();
        ui->lineEdit_APP->show();
        ui->label_APP->show();
    }
}
/***********************************************************
* 打开一个.bin或.hex格式文件，并将文件路径设置到界面文本框中
***********************************************************/
void MainWindow::SetESP32LineEditText(location_code_t lacation)
{
    //写文件
     //QString curPath=QDir::currentPath();//获取系统当前目录
     QString dlgTitle="打开一个文件"; //对话框标题
     //QString filter="(*.hex);;(*.bin);;所有文件(*.*)"; //文件过滤器
     QString filter="(*.bin)"; //文件过滤器
     //添加打开路径保留功能
      QSettings settings("RYMCU", "RYCOM");
      // 从 QSettings 中读取上一次的路径，如果没有则使用当前目录
     QString lastPath = settings.value("lastFilePath", QDir::currentPath()).toString();

     QString aFileName=QFileDialog::getOpenFileName(NULL,dlgTitle,lastPath,filter);
    if (aFileName.isEmpty())
    {
         QMessageBox::critical(this, "提示", "文件为空！");
         return;
    }
    //保存当前路径
    QFileInfo fileInfo(aFileName);
    QString dirPath = fileInfo.absolutePath();
    // 将当前选择的文件所在目录保存到 QSettings 中
    settings.setValue("lastFilePath", dirPath);

    switch (lacation) {
    case BOOT_COMBIN:
        ui->lineEdit_BOOT_Combine->setText(aFileName);
        break;
    case PART:
        ui->lineEdit_PART->setText(aFileName);
        break;
    case APP:
        ui->lineEdit_APP->setText(aFileName);
    default:
        break;
    }
}
void MainWindow::on_pushButton_Open_BOOT_Combine_clicked()
{
    SetESP32LineEditText(BOOT_COMBIN);
}

void MainWindow::on_pushButton_Open_PART_clicked()
{
    SetESP32LineEditText(PART);
}

void MainWindow::on_pushButton_Open_APP_clicked()
{
    SetESP32LineEditText(APP);
}
/***********************************************************
* 启动ESP32串口ISP
* step1 获取.bin数据并存储
* step2 是指设置RTS，DTR，使目标进入串口下载模式。esp32_enter_bootloader()
* step3 发送同步指令进行握手esp32_loader_sync_cmd()
* step4 改变串口波特率loader_change_baudrate_cmd()
* step5 获取目标芯片型号esp32_loader_detect_chip()
* step6 spi flash挂载esp32_loader_spi_attach()
* step7 读取flash大小esp_loader_flash_detect_size()
* step8 设置flash参数loader_spi_parameters()
* step9 擦除flash并下载代码esp32_flash_binary()
* step1 复位目标芯片esp32_laoder_reset_target()
***********************************************************/
void MainWindow::on_pushButton_ESP32_START_clicked()
{
    char error;
    //设置进入ISP标志
    if(ISisping != 0) return;//ISP过程中再次点击按钮，直接返回
    ISisping = 2;

    on_pushButtonOpen_clicked();//重启串口，复位CH343P状态，保证能正常下载
    on_pushButtonOpen_clicked();

    ISisping = 2;//重新打开会将ISisping清零，这里再次赋值为2(ESP32下载中)
    ui->groupBoxRev->setTitle("ESP32 ISP log");//清除屏幕,设置标题,清空串口接收缓冲区
    ui->TextRev->clear();

    error = ESP32BinProcess();if(error)return;//处理.bin数据,BOOT_Combine_ByteArray,PART_ByteArray,APP_ByteArray
     esp32_enter_bootloader();

     ui->TextRev->insertPlainText("------------------------进入ESP32 Bootloader-----------------------\r\n");
     ui->TextRev->insertPlainText("握手通信中...");//显示数据

     unsigned char StartTimes = 20;//握手20次
     while(StartTimes--)
     {
        if(ESP_LOADER_SUCCESS == esp32_loader_sync_cmd())  break;
        else   ui->TextRev->insertPlainText("...");
        delay_msec(300);
        if(StartTimes==1)
        {
          QMessageBox::critical(this, "提示", "进入ESP32 bootloader失败.\r\n请确认已进入下载模式!\r\n");
          ui->TextRev->insertPlainText("\r\n进入bootloader：失败!");
          ResumeFormESP32ISP();
          return;
        }
     }

    ui->TextRev->insertPlainText("\r\n------------------------bootloader握手成功-------------------------\r\n");
    //115200握手成功之后再切换波特率
    qDebug()<<"ESP32_loader_daud_before:"<<ESP32_loader_daud;
    if((115200 != ESP32_loader_daud) && (s_target != ESP8266_CHIP) )//ESP8266不允许改变波特率
    {
        if(ESP_LOADER_SUCCESS != loader_change_baudrate_cmd(ESP32_loader_daud,0))
        {
            QMessageBox::critical(this, "提示", "下载波特率设置失败!");
            ui->TextRev->insertPlainText("\r\n下载波特率设置失败!");
            ResumeFormESP32ISP();
            return;
        }
    }
    ui->TextRev->insertPlainText("串口波特率设为：");
    ui->TextRev->insertPlainText(QString::number(ESP32_loader_daud,10));
    qDebug()<<"ESP32_loader_daud_after:"<<ESP32_loader_daud;
    ui->TextRev->insertPlainText("bps\r\n");
    if(s_target == ESP8266_CHIP)ESP32_loader_daud = 115200;
    MyCom.setBaudRate(ESP32_loader_daud);
    qDebug()<<"reset baud:"<<ESP32_loader_daud;
    delay_msec(500);
    if(ESP_LOADER_SUCCESS != esp32_loader_detect_chip(&s_target, &s_reg))
    {
        qDebug() << "detect chip fail";
        ui->TextRev->insertPlainText("获取芯片型号失败!\r\n");
        ResumeFormESP32ISP();
        return;
    }
   qDebug() << "detect chip OK";
    PrintChipInfo();
    get_esp32_addr(s_target,&esp32_bin);
   if(ESP_LOADER_SUCCESS != esp32_loader_spi_attach(0,s_target))
   {
       ResumeFormESP32ISP();
       return;
   }
   qDebug() << "esp32_loader_spi_attach OK,s_target:"<<s_target;
   ui->TextRev->insertPlainText("\r\nspi flash挂载成功,");
   if(ESP_LOADER_SUCCESS != esp_loader_flash_detect_size(&s_target_flash_size))
   {
       qDebug() << "esp_loader_flash_detect_size fail"<<s_target;
       ResumeFormESP32ISP();
       return;
   }
   if(ESP_LOADER_SUCCESS != loader_spi_parameters(DEFAULT_FLASH_SIZE))//8M flash
   {
       qDebug() << "loader_spi_parameters set fail"<<s_target;
       ResumeFormESP32ISP();
       return;
   }
   uint32_t FLASHSIZE = s_target_flash_size/1024/1024;
   ui->TextRev->insertPlainText("容量为:");
   ui->TextRev->insertPlainText(QString::number(FLASHSIZE,10));
   ui->TextRev->insertPlainText("MB!");

   ui->TextRev->insertPlainText("\r\n参数设置成功!\r\n");

    ui->TextRev->insertPlainText("---------------------------开始下载-------------------------------\r\n");
    //esp32_flash_binary();

   if(ui->radioButton_combine->isChecked() == false)//常规3个.bin下载模式
   {
       ui->TextRev->insertPlainText("boot.bin:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.boot.size,10));
       ui->TextRev->insertPlainText("(Byte) addr:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.boot.addr,16));
       ui->TextRev->insertPlainText("\r\n擦除bootloader分区，并开始下载.....\r\n");
       error  = esp32_flash_binary(esp32_bin.boot.data, esp32_bin.boot.size, esp32_bin.boot.addr,BOOT_COMBIN);
       if(0==error)ui->TextRev->insertPlainText("bootloader分区下载成功!\r\n\r\n");
       qDebug()<<"esp32_bin.app.addr:"<<esp32_bin.app.addr;

       ui->TextRev->insertPlainText("part.bin:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.part.size,10));
       ui->TextRev->insertPlainText("(Byte) addr:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.part.addr,16));
       ui->TextRev->insertPlainText("\r\n擦除partition-table分区，并开始下载.....\r\n");
       error += esp32_flash_binary(esp32_bin.part.data, esp32_bin.part.size, esp32_bin.part.addr,PART);
       if(0==error)ui->TextRev->insertPlainText("partition-table分区下载成功!\r\n\r\n");

       ui->TextRev->insertPlainText("app.bin:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.app.size,10));
       ui->TextRev->insertPlainText("(Byte) addr:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.app.addr,16));
       ui->TextRev->insertPlainText("\r\n擦除application分区，并开始下载.....\r\n");
       error += esp32_flash_binary(esp32_bin.app.data,  esp32_bin.app.size,  esp32_bin.app.addr,APP);
        if(0==error)ui->TextRev->insertPlainText("application分区下载成功!\r\n");
   }
   else//合并.bin下载模式
   {
       ui->TextRev->insertPlainText("combine.bin:");
       ui->TextRev->insertPlainText(QString::number(esp32_bin.boot.size,10));
       ui->TextRev->insertPlainText("(Byte)\r\n");
       ui->TextRev->insertPlainText("擦除FLASH，并开始下载.....\r\n");
       error += esp32_flash_binary(esp32_bin.boot.data, esp32_bin.boot.size, 0,BOOT_COMBIN);//合并.bin时，下载地址设置为0
       if(0==error)ui->TextRev->insertPlainText("代码下载成功!\r\n");
   }
    if(error)
    {
        QMessageBox::critical(this, "提示", "下载失败!");
        ui->TextRev->insertPlainText("\r\n下载失败!");
        ResumeFormESP32ISP();
        return;
    }
     ui->TextRev->insertPlainText("------------------------下载成功!enjoy!---------------------------\r\n\r\n");
     ResumeFormESP32ISP();
     MyCom.setBaudRate(115200);
     //ISisping = 0;
     ui->TextRev->insertPlainText("串口助手波特率设置为：115200bps,并复位目标芯片!\r\n");
     ui->TextRev->insertPlainText("\r\n------------RYMCU嵌入式知识学习交流平台(rymcu.com)----------------\r\n\r\n");
    esp32_laoder_reset_target();
}
esp_loader_error_t MainWindow::esp32_flash_binary(const uint8_t *bin, size_t size, size_t address,location_code_t location)
{
    esp_loader_error_t err;
    static uint8_t payload[1024];
    const uint8_t *bin_addr = bin;

   qDebug()<<"Erasing flash (this may take a while)...";
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS)
    {
      //printf("esp_loader_flash_start fail:%d\n",err);
      qDebug()<<"esp_loader_flash_start fail";
      return err;
    }
    //printf("Start programming\n");
    qDebug()<<"Start programming....";
    size_t binary_size = size;
    size_t written = 0;
    qDebug()<<"binary_size:"<<size;
    switch (location) {
    case BOOT_COMBIN:
        ui->progressBar_BOOT_Combine->setVisible(true);
        ui->progressBar_BOOT_Combine->setMaximum(binary_size);
        ui->progressBar_BOOT_Combine->setValue(written);
        break;
    case PART:
        ui->progressBar_PART->setVisible(true);
        ui->progressBar_PART->setMaximum(binary_size);
        ui->progressBar_PART->setValue(written);
        break;
    case APP:
        ui->progressBar_APP->setVisible(true);
        ui->progressBar_APP->setMaximum(binary_size);
        ui->progressBar_APP->setValue(written);
        break;
    default:
        break;
    }

    while (size > 0)
    {
        size_t to_read = MIN(size, sizeof(payload));
        memcpy(payload, bin_addr, to_read);
        //qDebug()<<"payload[0]:"<<payload[0];
        //qDebug()<<"payload[1]:"<<payload[1];
        //qDebug()<<"payload[2]:"<<payload[2];

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            qDebug()<<"Packet could not be written"<<err;
            return err;
        }

        size -= to_read;
        bin_addr += to_read;
        written += to_read;

        //int progress = (int)(((float)written / binary_size) * 100);
       // printf("\rProgress: %d %%", progress);
        //qDebug()<<"progress:"<<progress;
        switch (location)
        {
            case BOOT_COMBIN:
                ui->progressBar_BOOT_Combine->setValue(written);
                break;
            case PART:
                ui->progressBar_PART->setValue(written);
                break;
            case APP:
                ui->progressBar_APP->setValue(written);
                break;
            default:
                break;
        }

    };

    qDebug()<<"Finished programming";

    return ESP_LOADER_SUCCESS;
}
void MainWindow::PrintChipInfo()
{
    switch (s_target) {
    case ESP8266_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP8266");
        break;
    case ESP32_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32");
        break;
    case ESP32S2_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32S2");
        break;
    case ESP32C3_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32C3");
        break;
    case ESP32S3_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32S3");
        break;
    case ESP32C2_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32C2");
        break;
    case ESP32H2_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32H2");
        break;
    case ESP32C6_CHIP:
        ui->TextRev->insertPlainText("检测到芯片型号：ESP32C6");
        break;
    default:
         ui->TextRev->insertPlainText("检测到芯片型号：不知道,lol");
        break;
    }
}
/***********************************************************
*
***********************************************************/
char MainWindow::ESP32BinProcess()
{
    char error = 0;

    error = OpenESP32bin(&BOOT_Combine_ByteArray,BOOT_COMBIN);

    esp32_bin.boot.data = (uint8_t *)BOOT_Combine_ByteArray.data();
    esp32_bin.boot.size = BOOT_Combine_ByteArray.size();
    qDebug()<<"BOOT_Combine_ByteArray.size:"<<esp32_bin.boot.size;

    if(ui->radioButton_combine->isChecked() == false)//未选中，需要下载3个bin
    {
        error += OpenESP32bin(&PART_ByteArray,PART);//PART.bin
        error += OpenESP32bin(&APP_ByteArray,APP);//app.bin

        esp32_bin.part.data = (uint8_t *)PART_ByteArray.data();
        esp32_bin.part.size = PART_ByteArray.size();
        qDebug()<<"PART_ByteArray.size:"<<esp32_bin.part.size;

        esp32_bin.app.data = (uint8_t *)APP_ByteArray.data();
        esp32_bin.app.size = APP_ByteArray.size();
        qDebug()<<"APP_ByteArray.size:"<<esp32_bin.app.size;

    }
    //get_esp32_addr(s_target,&esp32_bin);
    if(error)
    {
        QMessageBox::critical(this, "提示", "ESP32文件打开失败!");
        ui->groupBoxRev->setTitle("接收区");
        ISisping = 0;

        ui->progressBar_BOOT_Combine->setVisible(false);//隐藏进度条
        ui->progressBar_PART->setVisible(false);
        ui->progressBar_APP->setVisible(false);

        return 1;
    }

    return error;//0:正确
}
char MainWindow::OpenESP32bin(QByteArray *bindata,location_code_t location)
{
    QString aFileName;

    if(BOOT_COMBIN == location)aFileName = ui->lineEdit_BOOT_Combine->text();
    if(PART == location)aFileName = ui->lineEdit_PART->text();
    if(APP == location)aFileName = ui->lineEdit_APP->text();

    //ui->TextRev->insertPlainText(aFileName);
    if (!aFileName.contains(".bin"))
    {
         ui->TextRev->insertPlainText("!aFileName.contains(.bin)\r\n");
         return 1;//文件名不包含.bin,错误返回
    }
    QFile   aFile(aFileName);//用IODevice方式打开文本文件
    if (!aFile.exists())
    {
         ui->TextRev->insertPlainText("!aFile.exists()\r\n");
        return 1;//文件不存在
    }
    if (!aFile.open(QIODevice::ReadOnly))
    {
        return 1;
    }

    *bindata = aFile.readAll();
    aFile.close();
    if(bindata->isNull())
    {
        ui->TextRev->insertPlainText("bindata.isNull()\r\n");
        return 1;
    }
    qDebug()<<"bindata:"<<bindata->size();
    return 0;
}

void MainWindow::on_comboBoxCheck_ESP32_currentIndexChanged(int index)
{
    switch (index) {
    case 0://8266
    case 1://ESP32
    case 2://S2
         ui->label_BOOT_Combine->setText("@ 0x1000");//BOOTLOADER_ADDRESS_V0=0x1000
        break;
    default://其他芯片//BOOTLOADER_ADDRESS_V1=0x0
        ui->label_BOOT_Combine->setText("@ 0x0");
        break;
    }
}
