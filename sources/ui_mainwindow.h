/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "mycombobox.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEditTime;
    QLabel *labelMS;
    QCheckBox *checkBoxPeriodicSend;
    QGroupBox *groupBoxComSet;
    QPushButton *pushButtonOpen;
    myComboBox *comboBoxNo;
    QRadioButton *radioButton_led;
    QComboBox *comboBoxStop;
    QComboBox *comboBoxCheck;
    QComboBox *comboBoxComBaud;
    QComboBox *comboBoxData;
    QLabel *labelBaud;
    QLabel *labelData;
    QLabel *labelStop;
    QLabel *labelCheck;
    QGroupBox *groupBoxSendSet;
    QPushButton *pushButtonRdFile;
    QPushButton *pushButtonClearSend;
    QCheckBox *checkBoxSendHex;
    QCheckBox *checkBoxAddNewShift;
    QCheckBox *checkBoxAutoDownload;
    QGroupBox *groupBoxRevSet;
    QPushButton *pushButtonSaveRev;
    QPushButton *pushButtonStopRev;
    QPushButton *pushButtonClearRev;
    QCheckBox *checkBoxReVTime;
    QCheckBox *checkBoxRevHex;
    QGroupBox *groupBoxSend;
    QPlainTextEdit *TextSend;
    QPushButton *pushButtonSend;
    QGroupBox *groupBoxRev;
    QPlainTextEdit *TextRev;
    QRadioButton *radioButton;
    QGroupBox *groupBoxMutiSend;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEditMuti8;
    QCheckBox *checkBoxMuti_4;
    QLineEdit *lineEditMuti2;
    QPushButton *pushButtonMuti1;
    QLineEdit *lineEditMuti10;
    QCheckBox *checkBoxMuti_7;
    QCheckBox *checkBoxMuti_8;
    QPushButton *pushButtonMuti4;
    QCheckBox *checkBoxMuti_6;
    QCheckBox *checkBoxMuti_2;
    QLineEdit *lineEditMuti9;
    QLineEdit *lineEditMuti3;
    QPushButton *pushButtonMuti9;
    QCheckBox *checkBoxMuti_9;
    QLineEdit *lineEditMuti5;
    QPushButton *pushButtonMuti2;
    QLineEdit *lineEditMuti1;
    QPushButton *pushButtonMuti6;
    QCheckBox *checkBoxMuti_1;
    QPushButton *pushButtonMuti5;
    QLineEdit *lineEditMuti6;
    QPushButton *pushButtonMuti7;
    QCheckBox *checkBoxMuti_5;
    QLineEdit *lineEditMuti4;
    QPushButton *pushButtonMuti3;
    QCheckBox *checkBoxMuti_3;
    QPushButton *pushButtonMuti10;
    QPushButton *pushButtonMuti8;
    QCheckBox *checkBoxMuti_10;
    QLineEdit *lineEditMuti7;
    QPushButton *pushButtonMutiReset;
    QCheckBox *checkBoxPeriodicMutiSend;
    QPushButton *pushButton_RYISP;
    QPushButton *pushButton_EraseAll;
    QPushButton *pushButton_WriteBin;
    QPushButton *pushButton_HexToBin;
    QPushButton *pushButton_Ymodem;
    QPushButton *pushButton_ESP32ISP;
    QGroupBox *groupBox_esp32;
    QPushButton *pushButton_Open_BOOT_Combine;
    QPushButton *pushButton_Open_PART;
    QPushButton *pushButton_Open_APP;
    QLineEdit *lineEdit_BOOT_Combine;
    QLineEdit *lineEdit_PART;
    QLineEdit *lineEdit_APP;
    QComboBox *comboBoxCheck_ESP32;
    QRadioButton *radioButton_combine;
    QLabel *label_BOOT_Combine;
    QLabel *label_PART;
    QLabel *label_APP;
    QPushButton *pushButton_ESP32_START;
    QProgressBar *progressBar_PART;
    QProgressBar *progressBar_BOOT_Combine;
    QProgressBar *progressBar_APP;
    QGroupBox *groupBox_stm32;
    QPushButton *pushButton_STM32_START;
    QRadioButton *radioButton_EraseALL;
    QRadioButton *radioButton_NotErase;
    QLineEdit *lineEditMuti_binaddr;
    QPushButton *pushButton_OpenBin;
    QProgressBar *progressBar;
    QLineEdit *lineEditMuti_FileName;
    QLabel *label_PART_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(728, 944);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        MainWindow->setDockNestingEnabled(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lineEditTime = new QLineEdit(centralWidget);
        lineEditTime->setObjectName(QStringLiteral("lineEditTime"));
        lineEditTime->setGeometry(QRect(660, 372, 31, 16));
        QFont font;
        font.setPointSize(11);
        lineEditTime->setFont(font);
        labelMS = new QLabel(centralWidget);
        labelMS->setObjectName(QStringLiteral("labelMS"));
        labelMS->setGeometry(QRect(695, 372, 16, 16));
        QFont font1;
        font1.setPointSize(12);
        labelMS->setFont(font1);
        checkBoxPeriodicSend = new QCheckBox(centralWidget);
        checkBoxPeriodicSend->setObjectName(QStringLiteral("checkBoxPeriodicSend"));
        checkBoxPeriodicSend->setGeometry(QRect(580, 372, 81, 16));
        checkBoxPeriodicSend->setCheckable(true);
        groupBoxComSet = new QGroupBox(centralWidget);
        groupBoxComSet->setObjectName(QStringLiteral("groupBoxComSet"));
        groupBoxComSet->setGeometry(QRect(10, 0, 171, 201));
        pushButtonOpen = new QPushButton(groupBoxComSet);
        pushButtonOpen->setObjectName(QStringLiteral("pushButtonOpen"));
        pushButtonOpen->setGeometry(QRect(70, 170, 91, 22));
        comboBoxNo = new myComboBox(groupBoxComSet);
        comboBoxNo->setObjectName(QStringLiteral("comboBoxNo"));
        comboBoxNo->setGeometry(QRect(10, 30, 151, 22));
        comboBoxNo->setFont(font);
        comboBoxNo->setLayoutDirection(Qt::LeftToRight);
        comboBoxNo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        comboBoxNo->setMinimumContentsLength(0);
        radioButton_led = new QRadioButton(groupBoxComSet);
        radioButton_led->setObjectName(QStringLiteral("radioButton_led"));
        radioButton_led->setGeometry(QRect(20, 160, 31, 41));
        radioButton_led->setStyleSheet(QLatin1String("QRadioButton::indicator:checked{background-color:red;}\n"
"QRadioButton::indicator:unchecked{background-color:black;}\n"
"QRadioButton::indicator{width:25px;height:25px;border-radius:12px;}"));
        radioButton_led->setCheckable(true);
        radioButton_led->setChecked(false);
        radioButton_led->setAutoExclusive(false);
        comboBoxStop = new QComboBox(groupBoxComSet);
        comboBoxStop->setObjectName(QStringLiteral("comboBoxStop"));
        comboBoxStop->setGeometry(QRect(70, 113, 91, 22));
        comboBoxStop->setFont(font);
        comboBoxCheck = new QComboBox(groupBoxComSet);
        comboBoxCheck->setObjectName(QStringLiteral("comboBoxCheck"));
        comboBoxCheck->setGeometry(QRect(70, 139, 91, 22));
        comboBoxCheck->setFont(font);
        comboBoxComBaud = new QComboBox(groupBoxComSet);
        comboBoxComBaud->setObjectName(QStringLiteral("comboBoxComBaud"));
        comboBoxComBaud->setEnabled(true);
        comboBoxComBaud->setGeometry(QRect(70, 60, 91, 22));
        comboBoxComBaud->setFont(font);
        comboBoxData = new QComboBox(groupBoxComSet);
        comboBoxData->setObjectName(QStringLiteral("comboBoxData"));
        comboBoxData->setGeometry(QRect(70, 87, 91, 22));
        comboBoxData->setFont(font);
        comboBoxData->setLayoutDirection(Qt::LeftToRight);
        labelBaud = new QLabel(groupBoxComSet);
        labelBaud->setObjectName(QStringLiteral("labelBaud"));
        labelBaud->setGeometry(QRect(11, 61, 39, 18));
        labelData = new QLabel(groupBoxComSet);
        labelData->setObjectName(QStringLiteral("labelData"));
        labelData->setGeometry(QRect(11, 88, 39, 18));
        labelStop = new QLabel(groupBoxComSet);
        labelStop->setObjectName(QStringLiteral("labelStop"));
        labelStop->setGeometry(QRect(11, 115, 39, 18));
        labelCheck = new QLabel(groupBoxComSet);
        labelCheck->setObjectName(QStringLiteral("labelCheck"));
        labelCheck->setGeometry(QRect(11, 141, 39, 18));
        comboBoxStop->raise();
        comboBoxCheck->raise();
        comboBoxComBaud->raise();
        comboBoxData->raise();
        labelBaud->raise();
        labelData->raise();
        labelStop->raise();
        labelCheck->raise();
        pushButtonOpen->raise();
        comboBoxNo->raise();
        radioButton_led->raise();
        groupBoxSendSet = new QGroupBox(centralWidget);
        groupBoxSendSet->setObjectName(QStringLiteral("groupBoxSendSet"));
        groupBoxSendSet->setGeometry(QRect(10, 382, 171, 151));
        pushButtonRdFile = new QPushButton(groupBoxSendSet);
        pushButtonRdFile->setObjectName(QStringLiteral("pushButtonRdFile"));
        pushButtonRdFile->setGeometry(QRect(70, 25, 91, 22));
        pushButtonClearSend = new QPushButton(groupBoxSendSet);
        pushButtonClearSend->setObjectName(QStringLiteral("pushButtonClearSend"));
        pushButtonClearSend->setGeometry(QRect(70, 52, 91, 22));
        checkBoxSendHex = new QCheckBox(groupBoxSendSet);
        checkBoxSendHex->setObjectName(QStringLiteral("checkBoxSendHex"));
        checkBoxSendHex->setGeometry(QRect(76, 80, 101, 22));
        checkBoxAddNewShift = new QCheckBox(groupBoxSendSet);
        checkBoxAddNewShift->setObjectName(QStringLiteral("checkBoxAddNewShift"));
        checkBoxAddNewShift->setGeometry(QRect(76, 103, 101, 22));
        checkBoxAddNewShift->setCheckable(true);
        checkBoxAutoDownload = new QCheckBox(groupBoxSendSet);
        checkBoxAutoDownload->setObjectName(QStringLiteral("checkBoxAutoDownload"));
        checkBoxAutoDownload->setGeometry(QRect(76, 123, 101, 22));
        groupBoxRevSet = new QGroupBox(centralWidget);
        groupBoxRevSet->setObjectName(QStringLiteral("groupBoxRevSet"));
        groupBoxRevSet->setGeometry(QRect(10, 200, 171, 161));
        pushButtonSaveRev = new QPushButton(groupBoxRevSet);
        pushButtonSaveRev->setObjectName(QStringLiteral("pushButtonSaveRev"));
        pushButtonSaveRev->setGeometry(QRect(70, 30, 91, 22));
        pushButtonStopRev = new QPushButton(groupBoxRevSet);
        pushButtonStopRev->setObjectName(QStringLiteral("pushButtonStopRev"));
        pushButtonStopRev->setGeometry(QRect(70, 55, 91, 22));
        pushButtonClearRev = new QPushButton(groupBoxRevSet);
        pushButtonClearRev->setObjectName(QStringLiteral("pushButtonClearRev"));
        pushButtonClearRev->setGeometry(QRect(70, 79, 91, 22));
        checkBoxReVTime = new QCheckBox(groupBoxRevSet);
        checkBoxReVTime->setObjectName(QStringLiteral("checkBoxReVTime"));
        checkBoxReVTime->setGeometry(QRect(76, 114, 77, 22));
        checkBoxRevHex = new QCheckBox(groupBoxRevSet);
        checkBoxRevHex->setObjectName(QStringLiteral("checkBoxRevHex"));
        checkBoxRevHex->setGeometry(QRect(76, 137, 77, 22));
        checkBoxRevHex->setCheckable(true);
        checkBoxRevHex->setChecked(false);
        checkBoxRevHex->setTristate(false);
        groupBoxSend = new QGroupBox(centralWidget);
        groupBoxSend->setObjectName(QStringLiteral("groupBoxSend"));
        groupBoxSend->setGeometry(QRect(180, 382, 541, 151));
        TextSend = new QPlainTextEdit(groupBoxSend);
        TextSend->setObjectName(QStringLiteral("TextSend"));
        TextSend->setEnabled(true);
        TextSend->setGeometry(QRect(10, 30, 521, 111));
        pushButtonSend = new QPushButton(groupBoxSend);
        pushButtonSend->setObjectName(QStringLiteral("pushButtonSend"));
        pushButtonSend->setGeometry(QRect(500, 120, 31, 21));
        pushButtonSend->setCheckable(false);
        pushButtonSend->setChecked(false);
        pushButtonSend->setAutoDefault(false);
        groupBoxRev = new QGroupBox(centralWidget);
        groupBoxRev->setObjectName(QStringLiteral("groupBoxRev"));
        groupBoxRev->setGeometry(QRect(180, 0, 341, 361));
        TextRev = new QPlainTextEdit(groupBoxRev);
        TextRev->setObjectName(QStringLiteral("TextRev"));
        TextRev->setGeometry(QRect(10, 30, 321, 321));
        TextRev->setLayoutDirection(Qt::LeftToRight);
        TextRev->setReadOnly(true);
        radioButton = new QRadioButton(centralWidget);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(500, 372, 81, 16));
        radioButton->setAutoExclusive(false);
        groupBoxMutiSend = new QGroupBox(centralWidget);
        groupBoxMutiSend->setObjectName(QStringLiteral("groupBoxMutiSend"));
        groupBoxMutiSend->setGeometry(QRect(530, 0, 191, 361));
        label_3 = new QLabel(groupBoxMutiSend);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(160, 20, 31, 16));
        label = new QLabel(groupBoxMutiSend);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 41, 16));
        label_2 = new QLabel(groupBoxMutiSend);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(60, 20, 61, 16));
        lineEditMuti8 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti8->setObjectName(QStringLiteral("lineEditMuti8"));
        lineEditMuti8->setGeometry(QRect(30, 239, 121, 20));
        checkBoxMuti_4 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_4->setObjectName(QStringLiteral("checkBoxMuti_4"));
        checkBoxMuti_4->setGeometry(QRect(9, 130, 16, 16));
        lineEditMuti2 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti2->setObjectName(QStringLiteral("lineEditMuti2"));
        lineEditMuti2->setGeometry(QRect(30, 72, 121, 20));
        pushButtonMuti1 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti1->setObjectName(QStringLiteral("pushButtonMuti1"));
        pushButtonMuti1->setGeometry(QRect(157, 42, 31, 23));
        lineEditMuti10 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti10->setObjectName(QStringLiteral("lineEditMuti10"));
        lineEditMuti10->setGeometry(QRect(30, 299, 121, 20));
        checkBoxMuti_7 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_7->setObjectName(QStringLiteral("checkBoxMuti_7"));
        checkBoxMuti_7->setGeometry(QRect(9, 215, 16, 16));
        checkBoxMuti_8 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_8->setObjectName(QStringLiteral("checkBoxMuti_8"));
        checkBoxMuti_8->setGeometry(QRect(9, 242, 16, 16));
        pushButtonMuti4 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti4->setObjectName(QStringLiteral("pushButtonMuti4"));
        pushButtonMuti4->setGeometry(QRect(158, 126, 31, 23));
        checkBoxMuti_6 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_6->setObjectName(QStringLiteral("checkBoxMuti_6"));
        checkBoxMuti_6->setGeometry(QRect(9, 187, 16, 16));
        checkBoxMuti_2 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_2->setObjectName(QStringLiteral("checkBoxMuti_2"));
        checkBoxMuti_2->setGeometry(QRect(9, 75, 16, 16));
        lineEditMuti9 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti9->setObjectName(QStringLiteral("lineEditMuti9"));
        lineEditMuti9->setGeometry(QRect(31, 269, 121, 20));
        lineEditMuti3 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti3->setObjectName(QStringLiteral("lineEditMuti3"));
        lineEditMuti3->setGeometry(QRect(30, 100, 121, 20));
        pushButtonMuti9 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti9->setObjectName(QStringLiteral("pushButtonMuti9"));
        pushButtonMuti9->setGeometry(QRect(158, 267, 31, 23));
        checkBoxMuti_9 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_9->setObjectName(QStringLiteral("checkBoxMuti_9"));
        checkBoxMuti_9->setGeometry(QRect(10, 272, 16, 16));
        lineEditMuti5 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti5->setObjectName(QStringLiteral("lineEditMuti5"));
        lineEditMuti5->setGeometry(QRect(30, 156, 121, 20));
        pushButtonMuti2 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti2->setObjectName(QStringLiteral("pushButtonMuti2"));
        pushButtonMuti2->setGeometry(QRect(158, 70, 31, 23));
        lineEditMuti1 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti1->setObjectName(QStringLiteral("lineEditMuti1"));
        lineEditMuti1->setGeometry(QRect(30, 44, 121, 20));
        pushButtonMuti6 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti6->setObjectName(QStringLiteral("pushButtonMuti6"));
        pushButtonMuti6->setGeometry(QRect(158, 182, 31, 23));
        checkBoxMuti_1 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_1->setObjectName(QStringLiteral("checkBoxMuti_1"));
        checkBoxMuti_1->setGeometry(QRect(9, 47, 16, 16));
        pushButtonMuti5 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti5->setObjectName(QStringLiteral("pushButtonMuti5"));
        pushButtonMuti5->setGeometry(QRect(158, 154, 31, 23));
        lineEditMuti6 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti6->setObjectName(QStringLiteral("lineEditMuti6"));
        lineEditMuti6->setGeometry(QRect(30, 184, 121, 20));
        pushButtonMuti7 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti7->setObjectName(QStringLiteral("pushButtonMuti7"));
        pushButtonMuti7->setGeometry(QRect(158, 210, 31, 23));
        checkBoxMuti_5 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_5->setObjectName(QStringLiteral("checkBoxMuti_5"));
        checkBoxMuti_5->setGeometry(QRect(9, 159, 16, 16));
        lineEditMuti4 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti4->setObjectName(QStringLiteral("lineEditMuti4"));
        lineEditMuti4->setGeometry(QRect(30, 127, 121, 20));
        pushButtonMuti3 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti3->setObjectName(QStringLiteral("pushButtonMuti3"));
        pushButtonMuti3->setGeometry(QRect(158, 98, 31, 23));
        checkBoxMuti_3 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_3->setObjectName(QStringLiteral("checkBoxMuti_3"));
        checkBoxMuti_3->setGeometry(QRect(9, 103, 16, 16));
        pushButtonMuti10 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti10->setObjectName(QStringLiteral("pushButtonMuti10"));
        pushButtonMuti10->setGeometry(QRect(158, 297, 31, 23));
        pushButtonMuti8 = new QPushButton(groupBoxMutiSend);
        pushButtonMuti8->setObjectName(QStringLiteral("pushButtonMuti8"));
        pushButtonMuti8->setGeometry(QRect(158, 238, 31, 23));
        checkBoxMuti_10 = new QCheckBox(groupBoxMutiSend);
        checkBoxMuti_10->setObjectName(QStringLiteral("checkBoxMuti_10"));
        checkBoxMuti_10->setGeometry(QRect(9, 302, 16, 16));
        lineEditMuti7 = new QLineEdit(groupBoxMutiSend);
        lineEditMuti7->setObjectName(QStringLiteral("lineEditMuti7"));
        lineEditMuti7->setGeometry(QRect(30, 212, 121, 20));
        pushButtonMutiReset = new QPushButton(groupBoxMutiSend);
        pushButtonMutiReset->setObjectName(QStringLiteral("pushButtonMutiReset"));
        pushButtonMutiReset->setGeometry(QRect(10, 330, 31, 23));
        checkBoxPeriodicMutiSend = new QCheckBox(groupBoxMutiSend);
        checkBoxPeriodicMutiSend->setObjectName(QStringLiteral("checkBoxPeriodicMutiSend"));
        checkBoxPeriodicMutiSend->setGeometry(QRect(50, 330, 131, 23));
        checkBoxPeriodicMutiSend->setCheckable(true);
        pushButton_RYISP = new QPushButton(centralWidget);
        pushButton_RYISP->setObjectName(QStringLiteral("pushButton_RYISP"));
        pushButton_RYISP->setEnabled(true);
        pushButton_RYISP->setGeometry(QRect(260, 370, 91, 22));
        pushButton_RYISP->setAutoDefault(false);
        pushButton_EraseAll = new QPushButton(centralWidget);
        pushButton_EraseAll->setObjectName(QStringLiteral("pushButton_EraseAll"));
        pushButton_EraseAll->setGeometry(QRect(160, 690, 113, 32));
        pushButton_WriteBin = new QPushButton(centralWidget);
        pushButton_WriteBin->setObjectName(QStringLiteral("pushButton_WriteBin"));
        pushButton_WriteBin->setGeometry(QRect(40, 650, 113, 32));
        pushButton_HexToBin = new QPushButton(centralWidget);
        pushButton_HexToBin->setObjectName(QStringLiteral("pushButton_HexToBin"));
        pushButton_HexToBin->setGeometry(QRect(40, 690, 113, 32));
        pushButton_Ymodem = new QPushButton(centralWidget);
        pushButton_Ymodem->setObjectName(QStringLiteral("pushButton_Ymodem"));
        pushButton_Ymodem->setEnabled(false);
        pushButton_Ymodem->setGeometry(QRect(280, 700, 81, 21));
        pushButton_Ymodem->setAutoDefault(false);
        pushButton_ESP32ISP = new QPushButton(centralWidget);
        pushButton_ESP32ISP->setObjectName(QStringLiteral("pushButton_ESP32ISP"));
        pushButton_ESP32ISP->setEnabled(true);
        pushButton_ESP32ISP->setGeometry(QRect(370, 370, 91, 22));
        pushButton_ESP32ISP->setAutoDefault(false);
        groupBox_esp32 = new QGroupBox(centralWidget);
        groupBox_esp32->setObjectName(QStringLiteral("groupBox_esp32"));
        groupBox_esp32->setGeometry(QRect(180, 560, 531, 151));
        pushButton_Open_BOOT_Combine = new QPushButton(groupBox_esp32);
        pushButton_Open_BOOT_Combine->setObjectName(QStringLiteral("pushButton_Open_BOOT_Combine"));
        pushButton_Open_BOOT_Combine->setGeometry(QRect(120, 30, 101, 23));
        pushButton_Open_PART = new QPushButton(groupBox_esp32);
        pushButton_Open_PART->setObjectName(QStringLiteral("pushButton_Open_PART"));
        pushButton_Open_PART->setGeometry(QRect(120, 55, 101, 22));
        pushButton_Open_APP = new QPushButton(groupBox_esp32);
        pushButton_Open_APP->setObjectName(QStringLiteral("pushButton_Open_APP"));
        pushButton_Open_APP->setGeometry(QRect(120, 79, 101, 22));
        lineEdit_BOOT_Combine = new QLineEdit(groupBox_esp32);
        lineEdit_BOOT_Combine->setObjectName(QStringLiteral("lineEdit_BOOT_Combine"));
        lineEdit_BOOT_Combine->setGeometry(QRect(231, 32, 179, 21));
        lineEdit_PART = new QLineEdit(groupBox_esp32);
        lineEdit_PART->setObjectName(QStringLiteral("lineEdit_PART"));
        lineEdit_PART->setGeometry(QRect(231, 55, 179, 21));
        lineEdit_APP = new QLineEdit(groupBox_esp32);
        lineEdit_APP->setObjectName(QStringLiteral("lineEdit_APP"));
        lineEdit_APP->setGeometry(QRect(231, 78, 179, 21));
        comboBoxCheck_ESP32 = new QComboBox(groupBox_esp32);
        comboBoxCheck_ESP32->setObjectName(QStringLiteral("comboBoxCheck_ESP32"));
        comboBoxCheck_ESP32->setGeometry(QRect(10, 30, 91, 21));
        comboBoxCheck_ESP32->setFont(font);
        radioButton_combine = new QRadioButton(groupBox_esp32);
        radioButton_combine->setObjectName(QStringLiteral("radioButton_combine"));
        radioButton_combine->setGeometry(QRect(10, 60, 111, 16));
        radioButton_combine->setAutoExclusive(false);
        label_BOOT_Combine = new QLabel(groupBox_esp32);
        label_BOOT_Combine->setObjectName(QStringLiteral("label_BOOT_Combine"));
        label_BOOT_Combine->setGeometry(QRect(421, 31, 81, 16));
        label_PART = new QLabel(groupBox_esp32);
        label_PART->setObjectName(QStringLiteral("label_PART"));
        label_PART->setGeometry(QRect(421, 57, 64, 16));
        label_APP = new QLabel(groupBox_esp32);
        label_APP->setObjectName(QStringLiteral("label_APP"));
        label_APP->setGeometry(QRect(421, 82, 70, 16));
        pushButton_ESP32_START = new QPushButton(groupBox_esp32);
        pushButton_ESP32_START->setObjectName(QStringLiteral("pushButton_ESP32_START"));
        pushButton_ESP32_START->setGeometry(QRect(340, 100, 71, 20));
        progressBar_PART = new QProgressBar(groupBox_esp32);
        progressBar_PART->setObjectName(QStringLiteral("progressBar_PART"));
        progressBar_PART->setEnabled(true);
        progressBar_PART->setGeometry(QRect(230, 56, 181, 20));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(progressBar_PART->sizePolicy().hasHeightForWidth());
        progressBar_PART->setSizePolicy(sizePolicy1);
        progressBar_PART->setMinimumSize(QSize(0, 0));
        QFont font2;
        font2.setPointSize(13);
        progressBar_PART->setFont(font2);
        progressBar_PART->setMouseTracking(false);
        progressBar_PART->setTabletTracking(false);
        progressBar_PART->setContextMenuPolicy(Qt::DefaultContextMenu);
        progressBar_PART->setAcceptDrops(false);
        progressBar_PART->setAutoFillBackground(true);
        progressBar_PART->setValue(0);
        progressBar_PART->setAlignment(Qt::AlignCenter);
        progressBar_PART->setTextVisible(true);
        progressBar_PART->setInvertedAppearance(false);
        progressBar_PART->setTextDirection(QProgressBar::TopToBottom);
        progressBar_BOOT_Combine = new QProgressBar(groupBox_esp32);
        progressBar_BOOT_Combine->setObjectName(QStringLiteral("progressBar_BOOT_Combine"));
        progressBar_BOOT_Combine->setEnabled(true);
        progressBar_BOOT_Combine->setGeometry(QRect(230, 30, 181, 20));
        sizePolicy1.setHeightForWidth(progressBar_BOOT_Combine->sizePolicy().hasHeightForWidth());
        progressBar_BOOT_Combine->setSizePolicy(sizePolicy1);
        progressBar_BOOT_Combine->setMinimumSize(QSize(0, 0));
        progressBar_BOOT_Combine->setFont(font2);
        progressBar_BOOT_Combine->setMouseTracking(false);
        progressBar_BOOT_Combine->setTabletTracking(false);
        progressBar_BOOT_Combine->setContextMenuPolicy(Qt::DefaultContextMenu);
        progressBar_BOOT_Combine->setAcceptDrops(false);
        progressBar_BOOT_Combine->setAutoFillBackground(true);
        progressBar_BOOT_Combine->setValue(0);
        progressBar_BOOT_Combine->setAlignment(Qt::AlignCenter);
        progressBar_BOOT_Combine->setTextVisible(false);
        progressBar_BOOT_Combine->setInvertedAppearance(false);
        progressBar_BOOT_Combine->setTextDirection(QProgressBar::TopToBottom);
        progressBar_APP = new QProgressBar(groupBox_esp32);
        progressBar_APP->setObjectName(QStringLiteral("progressBar_APP"));
        progressBar_APP->setEnabled(true);
        progressBar_APP->setGeometry(QRect(230, 80, 181, 19));
        sizePolicy1.setHeightForWidth(progressBar_APP->sizePolicy().hasHeightForWidth());
        progressBar_APP->setSizePolicy(sizePolicy1);
        progressBar_APP->setMinimumSize(QSize(0, 0));
        progressBar_APP->setFont(font2);
        progressBar_APP->setMouseTracking(false);
        progressBar_APP->setTabletTracking(false);
        progressBar_APP->setContextMenuPolicy(Qt::DefaultContextMenu);
        progressBar_APP->setAcceptDrops(false);
        progressBar_APP->setAutoFillBackground(true);
        progressBar_APP->setValue(0);
        progressBar_APP->setAlignment(Qt::AlignCenter);
        progressBar_APP->setTextVisible(true);
        progressBar_APP->setInvertedAppearance(false);
        progressBar_APP->setTextDirection(QProgressBar::TopToBottom);
        lineEdit_APP->raise();
        lineEdit_PART->raise();
        lineEdit_BOOT_Combine->raise();
        pushButton_Open_BOOT_Combine->raise();
        pushButton_Open_PART->raise();
        pushButton_Open_APP->raise();
        comboBoxCheck_ESP32->raise();
        radioButton_combine->raise();
        label_BOOT_Combine->raise();
        label_PART->raise();
        label_APP->raise();
        pushButton_ESP32_START->raise();
        progressBar_PART->raise();
        progressBar_BOOT_Combine->raise();
        progressBar_APP->raise();
        groupBox_stm32 = new QGroupBox(centralWidget);
        groupBox_stm32->setObjectName(QStringLiteral("groupBox_stm32"));
        groupBox_stm32->setGeometry(QRect(180, 720, 531, 151));
        pushButton_STM32_START = new QPushButton(groupBox_stm32);
        pushButton_STM32_START->setObjectName(QStringLiteral("pushButton_STM32_START"));
        pushButton_STM32_START->setGeometry(QRect(340, 90, 71, 21));
        radioButton_EraseALL = new QRadioButton(groupBox_stm32);
        radioButton_EraseALL->setObjectName(QStringLiteral("radioButton_EraseALL"));
        radioButton_EraseALL->setGeometry(QRect(10, 40, 71, 16));
        radioButton_EraseALL->setChecked(true);
        radioButton_NotErase = new QRadioButton(groupBox_stm32);
        radioButton_NotErase->setObjectName(QStringLiteral("radioButton_NotErase"));
        radioButton_NotErase->setGeometry(QRect(10, 60, 61, 16));
        lineEditMuti_binaddr = new QLineEdit(groupBox_stm32);
        lineEditMuti_binaddr->setObjectName(QStringLiteral("lineEditMuti_binaddr"));
        lineEditMuti_binaddr->setGeometry(QRect(330, 30, 81, 21));
        lineEditMuti_binaddr->setFont(font);
        pushButton_OpenBin = new QPushButton(groupBox_stm32);
        pushButton_OpenBin->setObjectName(QStringLiteral("pushButton_OpenBin"));
        pushButton_OpenBin->setGeometry(QRect(180, 60, 61, 21));
        progressBar = new QProgressBar(groupBox_stm32);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(250, 60, 161, 21));
        sizePolicy1.setHeightForWidth(progressBar->sizePolicy().hasHeightForWidth());
        progressBar->setSizePolicy(sizePolicy1);
        progressBar->setMinimumSize(QSize(0, 0));
        progressBar->setFont(font2);
        progressBar->setMouseTracking(false);
        progressBar->setTabletTracking(false);
        progressBar->setContextMenuPolicy(Qt::DefaultContextMenu);
        progressBar->setAcceptDrops(false);
        progressBar->setAutoFillBackground(true);
        progressBar->setValue(0);
        progressBar->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        progressBar->setTextVisible(true);
        progressBar->setInvertedAppearance(false);
        progressBar->setTextDirection(QProgressBar::TopToBottom);
        lineEditMuti_FileName = new QLineEdit(groupBox_stm32);
        lineEditMuti_FileName->setObjectName(QStringLiteral("lineEditMuti_FileName"));
        lineEditMuti_FileName->setGeometry(QRect(250, 60, 161, 21));
        label_PART_2 = new QLabel(groupBox_stm32);
        label_PART_2->setObjectName(QStringLiteral("label_PART_2"));
        label_PART_2->setGeometry(QRect(180, 30, 91, 16));
        lineEditMuti_FileName->raise();
        progressBar->raise();
        pushButton_STM32_START->raise();
        radioButton_EraseALL->raise();
        radioButton_NotErase->raise();
        lineEditMuti_binaddr->raise();
        pushButton_OpenBin->raise();
        label_PART_2->raise();
        MainWindow->setCentralWidget(centralWidget);
        pushButton_EraseAll->raise();
        labelMS->raise();
        checkBoxPeriodicSend->raise();
        groupBoxComSet->raise();
        groupBoxSendSet->raise();
        groupBoxRevSet->raise();
        groupBoxSend->raise();
        groupBoxRev->raise();
        radioButton->raise();
        groupBoxMutiSend->raise();
        lineEditTime->raise();
        pushButton_RYISP->raise();
        pushButton_WriteBin->raise();
        pushButton_HexToBin->raise();
        pushButton_Ymodem->raise();
        pushButton_ESP32ISP->raise();
        groupBox_esp32->raise();
        groupBox_stm32->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 728, 36));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setSizeGripEnabled(true);
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        comboBoxComBaud->setCurrentIndex(0);
        comboBoxData->setCurrentIndex(3);
        pushButtonSend->setDefault(false);
        pushButton_RYISP->setDefault(false);
        pushButton_Ymodem->setDefault(true);
        pushButton_ESP32ISP->setDefault(false);
        comboBoxCheck_ESP32->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "RYCOM\344\270\262\345\217\243\350\260\203\350\257\225\345\212\251\346\211\213V2.6.2 rymcu.com\345\265\214\345\205\245\345\274\217\347\237\245\350\257\206\345\255\246\344\271\240\344\272\244\346\265\201\345\271\263\345\217\260", Q_NULLPTR));
        lineEditTime->setText(QApplication::translate("MainWindow", "1000", Q_NULLPTR));
        labelMS->setText(QApplication::translate("MainWindow", "ms", Q_NULLPTR));
        checkBoxPeriodicSend->setText(QApplication::translate("MainWindow", "\345\221\250\346\234\237\345\217\221\351\200\201", Q_NULLPTR));
        groupBoxComSet->setTitle(QApplication::translate("MainWindow", "\344\270\262\345\217\243\350\256\276\347\275\256", Q_NULLPTR));
        pushButtonOpen->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", Q_NULLPTR));
        radioButton_led->setText(QString());
        comboBoxStop->clear();
        comboBoxStop->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1.5", Q_NULLPTR)
         << QApplication::translate("MainWindow", "2", Q_NULLPTR)
        );
        comboBoxCheck->clear();
        comboBoxCheck->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "None", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Even", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Odd", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Mark", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Space", Q_NULLPTR)
        );
        comboBoxComBaud->clear();
        comboBoxComBaud->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "115200", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1200", Q_NULLPTR)
         << QApplication::translate("MainWindow", "2400", Q_NULLPTR)
         << QApplication::translate("MainWindow", "4800", Q_NULLPTR)
         << QApplication::translate("MainWindow", "9600", Q_NULLPTR)
         << QApplication::translate("MainWindow", "19200", Q_NULLPTR)
         << QApplication::translate("MainWindow", "38400", Q_NULLPTR)
         << QApplication::translate("MainWindow", "57600", Q_NULLPTR)
         << QApplication::translate("MainWindow", "115200", Q_NULLPTR)
         << QApplication::translate("MainWindow", "230400", Q_NULLPTR)
         << QApplication::translate("MainWindow", "460800", Q_NULLPTR)
         << QApplication::translate("MainWindow", "576000", Q_NULLPTR)
         << QApplication::translate("MainWindow", "921600", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1382400", Q_NULLPTR)
         << QApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211", Q_NULLPTR)
        );
        comboBoxData->clear();
        comboBoxData->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "5", Q_NULLPTR)
         << QApplication::translate("MainWindow", "6", Q_NULLPTR)
         << QApplication::translate("MainWindow", "7", Q_NULLPTR)
         << QApplication::translate("MainWindow", "8", Q_NULLPTR)
        );
        labelBaud->setText(QApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", Q_NULLPTR));
        labelData->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", Q_NULLPTR));
        labelStop->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", Q_NULLPTR));
        labelCheck->setText(QApplication::translate("MainWindow", "\346\240\241\351\252\214\344\275\215", Q_NULLPTR));
        groupBoxSendSet->setTitle(QApplication::translate("MainWindow", "\345\217\221\351\200\201\350\256\276\347\275\256", Q_NULLPTR));
        pushButtonRdFile->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\226\346\226\207\344\273\266", Q_NULLPTR));
        pushButtonClearSend->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\345\217\221\351\200\201\345\214\272", Q_NULLPTR));
        checkBoxSendHex->setText(QApplication::translate("MainWindow", "\345\215\201\345\205\255\350\277\233\345\210\266", Q_NULLPTR));
        checkBoxAddNewShift->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\346\226\260\350\241\214", Q_NULLPTR));
        checkBoxAutoDownload->setText(QApplication::translate("MainWindow", "\350\207\252\345\220\257\345\212\250\344\270\213\350\275\275", Q_NULLPTR));
        groupBoxRevSet->setTitle(QApplication::translate("MainWindow", "\346\216\245\346\224\266\350\256\276\347\275\256", Q_NULLPTR));
        pushButtonSaveRev->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\346\226\207\344\273\266", Q_NULLPTR));
        pushButtonStopRev->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\346\230\276\347\244\272", Q_NULLPTR));
        pushButtonClearRev->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\346\216\245\346\224\266\345\214\272", Q_NULLPTR));
        checkBoxReVTime->setText(QApplication::translate("MainWindow", "\346\216\245\346\224\266\346\227\266\351\227\264", Q_NULLPTR));
        checkBoxRevHex->setText(QApplication::translate("MainWindow", "\345\215\201\345\205\255\350\277\233\345\210\266", Q_NULLPTR));
        groupBoxSend->setTitle(QApplication::translate("MainWindow", "\345\217\221\351\200\201\345\214\272", Q_NULLPTR));
        TextSend->setPlainText(QString());
        pushButtonSend->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", Q_NULLPTR));
        groupBoxRev->setTitle(QApplication::translate("MainWindow", "\346\216\245\346\224\266\345\214\272", Q_NULLPTR));
        TextRev->setPlainText(QString());
        radioButton->setText(QApplication::translate("MainWindow", "\345\244\232\350\241\214\345\217\221\351\200\201", Q_NULLPTR));
        groupBoxMutiSend->setTitle(QApplication::translate("MainWindow", "\345\244\232\350\241\214\345\217\221\351\200\201\345\214\272", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "\351\200\211\344\270\255", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "\350\276\223\345\205\245\345\255\227\347\254\246", Q_NULLPTR));
        lineEditMuti8->setText(QApplication::translate("MainWindow", "8", Q_NULLPTR));
        checkBoxMuti_4->setText(QString());
        lineEditMuti2->setText(QApplication::translate("MainWindow", "2", Q_NULLPTR));
        pushButtonMuti1->setText(QApplication::translate("MainWindow", "1", Q_NULLPTR));
        lineEditMuti10->setText(QApplication::translate("MainWindow", "10", Q_NULLPTR));
        checkBoxMuti_7->setText(QString());
        checkBoxMuti_8->setText(QString());
        pushButtonMuti4->setText(QApplication::translate("MainWindow", "4", Q_NULLPTR));
        checkBoxMuti_6->setText(QString());
        checkBoxMuti_2->setText(QString());
        lineEditMuti9->setText(QApplication::translate("MainWindow", "9", Q_NULLPTR));
        lineEditMuti3->setText(QApplication::translate("MainWindow", "3", Q_NULLPTR));
        pushButtonMuti9->setText(QApplication::translate("MainWindow", "9", Q_NULLPTR));
        checkBoxMuti_9->setText(QString());
        lineEditMuti5->setText(QApplication::translate("MainWindow", "5", Q_NULLPTR));
        pushButtonMuti2->setText(QApplication::translate("MainWindow", "2", Q_NULLPTR));
        lineEditMuti1->setText(QApplication::translate("MainWindow", "1", Q_NULLPTR));
        pushButtonMuti6->setText(QApplication::translate("MainWindow", "6", Q_NULLPTR));
        checkBoxMuti_1->setText(QString());
        pushButtonMuti5->setText(QApplication::translate("MainWindow", "5", Q_NULLPTR));
        lineEditMuti6->setText(QApplication::translate("MainWindow", "6", Q_NULLPTR));
        pushButtonMuti7->setText(QApplication::translate("MainWindow", "7", Q_NULLPTR));
        checkBoxMuti_5->setText(QString());
        lineEditMuti4->setText(QApplication::translate("MainWindow", "4", Q_NULLPTR));
        pushButtonMuti3->setText(QApplication::translate("MainWindow", "3", Q_NULLPTR));
        checkBoxMuti_3->setText(QString());
        pushButtonMuti10->setText(QApplication::translate("MainWindow", "10", Q_NULLPTR));
        pushButtonMuti8->setText(QApplication::translate("MainWindow", "8", Q_NULLPTR));
        checkBoxMuti_10->setText(QString());
        lineEditMuti7->setText(QApplication::translate("MainWindow", "7", Q_NULLPTR));
        pushButtonMutiReset->setText(QApplication::translate("MainWindow", "\345\244\215\344\275\215", Q_NULLPTR));
        checkBoxPeriodicMutiSend->setText(QApplication::translate("MainWindow", "\345\244\232\350\241\214\345\276\252\347\216\257\345\217\221\351\200\201", Q_NULLPTR));
        pushButton_RYISP->setText(QApplication::translate("MainWindow", "STM32\344\270\213\350\275\275", Q_NULLPTR));
        pushButton_EraseAll->setText(QApplication::translate("MainWindow", "EraseAll", Q_NULLPTR));
        pushButton_WriteBin->setText(QApplication::translate("MainWindow", "WriteBin", Q_NULLPTR));
        pushButton_HexToBin->setText(QApplication::translate("MainWindow", "HexToBin", Q_NULLPTR));
        pushButton_Ymodem->setText(QApplication::translate("MainWindow", "Ymodem", Q_NULLPTR));
        pushButton_ESP32ISP->setText(QApplication::translate("MainWindow", "ESP32\344\270\213\350\275\275", Q_NULLPTR));
        groupBox_esp32->setTitle(QApplication::translate("MainWindow", "ESP32\350\256\276\347\275\256", Q_NULLPTR));
        pushButton_Open_BOOT_Combine->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200BOOT", Q_NULLPTR));
        pushButton_Open_PART->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200PART", Q_NULLPTR));
        pushButton_Open_APP->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200APP", Q_NULLPTR));
        lineEdit_BOOT_Combine->setText(QString());
        lineEdit_BOOT_Combine->setPlaceholderText(QApplication::translate("MainWindow", "\351\200\211\346\213\251bootlaoder.bin", Q_NULLPTR));
        lineEdit_PART->setText(QString());
        lineEdit_PART->setPlaceholderText(QApplication::translate("MainWindow", "\351\200\211\346\213\251partition-table.bin", Q_NULLPTR));
        lineEdit_APP->setText(QString());
        lineEdit_APP->setPlaceholderText(QApplication::translate("MainWindow", "\351\200\211\346\213\251app.bin", Q_NULLPTR));
        comboBoxCheck_ESP32->clear();
        comboBoxCheck_ESP32->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "ESP8266", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32S2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32C3", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32S3", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32C2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32_RESERVED0_CHIP", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32H2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ESP32C6", Q_NULLPTR)
        );
        radioButton_combine->setText(QApplication::translate("MainWindow", "\345\215\225\344\270\252\345\220\210\345\271\266\345\220\216bin", Q_NULLPTR));
        label_BOOT_Combine->setText(QApplication::translate("MainWindow", "@ 0x0", Q_NULLPTR));
        label_PART->setText(QApplication::translate("MainWindow", "@ 0x8000", Q_NULLPTR));
        label_APP->setText(QApplication::translate("MainWindow", "@ 0x10000", Q_NULLPTR));
        pushButton_ESP32_START->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250\344\270\213\350\275\275", Q_NULLPTR));
        groupBox_stm32->setTitle(QApplication::translate("MainWindow", "STM32\350\256\276\347\275\256", Q_NULLPTR));
        pushButton_STM32_START->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250\344\270\213\350\275\275", Q_NULLPTR));
        radioButton_EraseALL->setText(QApplication::translate("MainWindow", "\345\205\250\346\223\246\351\231\244", Q_NULLPTR));
        radioButton_NotErase->setText(QApplication::translate("MainWindow", "\344\270\215\346\223\246\351\231\244", Q_NULLPTR));
        lineEditMuti_binaddr->setText(QApplication::translate("MainWindow", "0x08000000", Q_NULLPTR));
        pushButton_OpenBin->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", Q_NULLPTR));
        lineEditMuti_FileName->setText(QString());
        lineEditMuti_FileName->setPlaceholderText(QApplication::translate("MainWindow", "\351\200\211\346\213\251.Hex\346\210\226.bin\346\226\207\344\273\266", Q_NULLPTR));
        label_PART_2->setText(QApplication::translate("MainWindow", ".bin\350\265\267\345\247\213\345\234\260\345\235\200", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
