#-------------------------------------------------
#
# Project created by QtCreator 2021-01-27T18:59:42
#
#-------------------------------------------------

#添加串口支持
QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RYCOM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#for win
RC_ICONS = rymculogo.ico
# for mac
ICON = rymculogo.icns
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mycombobox.cpp \
    ryisp.cpp \
    mycom.cpp \
    HexToBin.cpp \
    ry_ymodem.cpp \
    myfilewatcher.cpp \
    ryesp32isp.cpp

HEADERS += \
        mainwindow.h \
    mycombobox.h \
    ryisp.h \
    mycom.h \
    HexToBin.h \
    HexToBin.h \
    ry_ymodem.h \
    myfilewatcher.h \
    ryesp32isp.h \
    esp32/esp32_protocol.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    rycomres.qrc
