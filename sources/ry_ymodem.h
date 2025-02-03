#ifndef RY_YMODEM_H
#define RY_YMODEM_H
/************************************************************
*             STM32串口ISP驱动
* ***********************************************************
* 【版  本】： V1.0
* 【作  者】： hugh
* 【网  站】：https://rymcu.com/
* 【邮  箱】：
*
* 【版  权】All Rights Reserved
* 【声  明】此程序仅用于学习与参考，引用请注明版权和作者信息！
* 【功  能】
*  实现Ymodem发送功能
*
************************************************************/
#include <mycom.h>
#include <stdint.h>
#include <QFile>
#include <QFileInfo>
#include<qdebug.h>
#include<string.h>

#define YMODEM_PACKET_HEADER    (3)
#define YMODEM_PACKET_TRAILER   (2)
#define YMODEM_PACKET_OVERHEAD  (YMODEM_PACKET_HEADER + YMODEM_PACKET_TRAILER)
#define YMODEM_PACKET_SIZE      (128)
#define YMODEM_PACKET_1K_SIZE   (1024)

/******************************
 * 1.发送开始，等待CodeC,超时退出
 * 2.发送SOH,等待ACK,等待CodeC，超时退出
 * 4.发送地1-N帧数据，等待ACK
 * 5.发送CodeEot,等待NAK
 * 6.发送CodeEot，等待ACK
 * 7.等待CodeC,发送结束
 * 8.发送结束
 * ******************************/

class RY_Ymodem
{
public:
   uint8_t  TxBuffer[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD];
   uint32_t TxLength;
   QByteArray FileBuffer;
   QFileInfo info;
   uint32_t FileSize;
   uint32_t YmodemProgress;

public:
  #define  TmsWatingForReceiver 10000
  #define TmsWatingForACK_NAK_C 500
  /*
  #define CodeNone 0x00
  #define CodeSoh  0x01
  #define CodeStx  0x02
  #define CodeEot  0x04
  #define CodeAck  0x06
  #define CodeNak  0x15
  #define CodeCan  0x18
  #define CodeC    0x43
  #define CodeA1   0x41
  #define CodeA2   0x61
*/
  enum Code_Ymodem
  {
    CodeNone = 0x00,
    CodeSoh  = 0x01,
    CodeStx  = 0x02,
    CodeEot  = 0x04,
    CodeAck  = 0x06,
    CodeNak  = 0x15,
    CodeCan  = 0x18,
    CodeC    = 0x43,
    CodeA1   = 0x41,
    CodeA2   = 0x61
  };

  char TxStart(void);
  char TxSoh(const QString FilePath);
  char TxData(uint32_t i);
  char TxEot(void);
  char TxEnd(void);
  void CPPtest(void);

private:
  char GetByte(void);//从串口接收一个字节
  char ACK(int delay_ms);//接收到从机应答0x79,返回1
  void SendData_Ymodem(char* Buf,uint32_t Len);//串口发送一个字节

  uint16_t crc16(uint8_t *buff, uint32_t len);
  void WriteCRC16(uint16_t size);//最后两个字节填充CRC16校验
};

//RY_Ymodem rymode;
#endif // RY_YMODEM_H
