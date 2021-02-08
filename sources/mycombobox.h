#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <QMouseEvent>
#include <QSerialPort>
#include <QSerialPortInfo>

class myComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit myComboBox(QWidget *parent = nullptr);

    // 重写鼠标点击事件
    void mousePressEvent(QMouseEvent *event);

signals:

private:
    void scanActivePort();

};

#endif // MYCOMBOBOX_H
