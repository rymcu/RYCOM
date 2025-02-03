/* Header includes -----------------------------------------------------------*/
#include "ry_ymodem.h"
#include <QFile>
#include <QFileInfo>
#include<qdebug.h>
#include<string.h>

/////////////////////////////////////////////////
//
//1.Ymodem传送开始，等待CodeC,超时退出
//返回值：
// 0:接收端准备好
// 1:等待超时
/////////////////////////////////////////////////
char RY_Ymodem::TxStart(void)
{
    //等待接收端准备好
    char Code;
    Code = ACK(TmsWatingForReceiver);
    if(Code == CodeC) return 0;
    else return 1;
}
/////////////////////////////////////////////////
//
//2.发送SOH,等待ACK,等待CodeC，超时退出
//返回值：
// 0:接收端准备好
// 1:等待超时
/////////////////////////////////////////////////
char RY_Ymodem::TxSoh(const QString FilePath)
{
    QString FileName,FileSizeQStr;

    //打开文件
     if (FilePath.isEmpty()) return 1;//文件路径为空
     QFile aFile(FilePath);//用IODevice方式打开文本文件
     if (!aFile.exists()) return 2;//文件不存在
     if (!aFile.open(QIODevice::ReadOnly))return 3;//文件打开失败

     FileBuffer= aFile.readAll();//文件内容存成员变量中（QByteArray型）
     aFile.close();

     //获取文件名称，大小
     info = QFileInfo(FilePath);
     FileName = info.fileName();//获得文件名字
     FileSize = info.size();//文件大小
     FileSizeQStr = QString::number(FileSize);//文件大小转换为Qstring,//qDebug("SizeQStr:%s, size:%d",qPrintable(FileSizeQStr),FileSize);

     //将文件名称，大小转换为char*
     std::string StrFileName = FileName.toStdString();
     std::string StrFileSize = FileSizeQStr.toStdString();
     const char* FileNameToChar= StrFileName.c_str();
     const char* FileSizeToChar = StrFileSize.c_str();

     size_t FileNameLength = strlen(FileNameToChar);//文件名称字符串的字节数
     size_t FileSizeLength = strlen(FileSizeToChar);//文件长度字符串的字节数

    //填充Ymodem首包数据，数据结构：SOH + 00 + FF + "ry.bin" + "123" + CRC + CRC
     TxBuffer[0] = CodeSoh;
     TxBuffer[1] = 0x00;
     TxBuffer[2] = 0xFF;

     strcpy((char *)TxBuffer+3, FileNameToChar);//文件名称，strcpy会将结束符\0拷贝进去,符合Ymodem文件名结束要求。
     strcpy((char *)(TxBuffer+3+FileNameLength+1), FileSizeToChar);//文件大小字符串填充

     uint16_t SohHeadLength = FileNameLength + FileSizeLength + 3 + 2;//文件名长度+文件大小字符长度+2个结束符+前3个固定值
     for(int i = SohHeadLength;i<YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD-2;i++)
     {
         TxBuffer[i] = 0x00;//填充0
     }
     //校验
        WriteCRC16(YMODEM_PACKET_SIZE);

     qDebug("Name:%s(%dByte),Size:%s(%dByte)",(char *)TxBuffer+3,FileNameLength,(char *)(TxBuffer+3+FileNameLength+1),FileSizeLength);

     //发送Soh
     SendData_Ymodem((char *)TxBuffer,YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD);

     //等待接收端准备好
     if(CodeAck != ACK(TmsWatingForACK_NAK_C)) return 1;//超时ACK
     if(CodeC   != ACK(TmsWatingForACK_NAK_C)) return 1;//超时C

     return 0;
}


char RY_Ymodem::TxData(uint32_t i)
{

        uint32_t FileSizeCnt = FileSize/YMODEM_PACKET_1K_SIZE;
        uint32_t FileSizeRem = FileSize%YMODEM_PACKET_1K_SIZE;
        //发送第一包数据，需要数据长度
        uint8_t *FileToChar = (uint8_t*)FileBuffer.data();

        TxBuffer[0] = CodeStx;
        TxBuffer[1] = i+1;
        TxBuffer[2] = ~TxBuffer[1];
        if(i!= FileSizeCnt)
        {//整数包
            memcpy(TxBuffer+3,FileToChar + YMODEM_PACKET_1K_SIZE*i,YMODEM_PACKET_1K_SIZE);
        }
        else
        {//不足YMODEM_PACKET_1K_SIZE长度包
            if(FileSizeRem)
            {
                memcpy(TxBuffer+3,FileToChar + YMODEM_PACKET_1K_SIZE*FileSizeCnt,FileSizeRem);
                for(int32_t i=FileSizeRem+3;i<YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD-2;i++){TxBuffer[i] = 0x1A;}
            }
        }

        WriteCRC16(YMODEM_PACKET_1K_SIZE);
        SendData_Ymodem((char *)TxBuffer,YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD);
        if(CodeAck != ACK(TmsWatingForACK_NAK_C)){return 1;}//超时ACK}
        YmodemProgress = i;
        return 0;
}

char RY_Ymodem::TxEot(void)
{
    char Ceot = CodeEot;
    SendData_Ymodem(&Ceot,1);
    if(CodeNak != ACK(TmsWatingForACK_NAK_C)) return 1;//超时NAK
    SendData_Ymodem(&Ceot,1);
    if(CodeAck != ACK(TmsWatingForACK_NAK_C)) return 1;//超时ACK
    return 0;
}
char RY_Ymodem::TxEnd(void)
{
    if(CodeC != ACK(TmsWatingForACK_NAK_C)) return 1;//超时C
    //填充Ymodem结束包数据，数据结构：SOH + 00 + FF + 00[128] + CRC + CRC
     TxBuffer[0] = CodeSoh;
     TxBuffer[1] = 0x00;
     TxBuffer[2] = 0xFF;

     for(int i = 3;i<YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD-2;i++)
     {
         TxBuffer[ i] = 0x00;//填充0
     }
     WriteCRC16(YMODEM_PACKET_SIZE);
     //发送end
     SendData_Ymodem((char *)TxBuffer,YMODEM_PACKET_SIZE + YMODEM_PACKET_OVERHEAD);
     if(CodeAck != ACK(TmsWatingForACK_NAK_C)) return 1;//超时ACK
     return 0;
}
/////////////////////////////////////////////////
//
//读取一Byte串口数据，并将后续数据前移
//
/////////////////////////////////////////////////
char RY_Ymodem::GetByte(void)
{
    char temp;
    temp = MyComRevBUff.at(0);
    MyComRevBUff.remove(0,1);
    return temp;
}
/////////////////////////////////////////////////
//
//串口发送Len字节数据
//
/////////////////////////////////////////////////
void RY_Ymodem::SendData_Ymodem(char* Buf,uint32_t Len)
{
    MyCom.write(Buf,Len);//mycom.h声明了MyCom串口对象
}
/////////////////////////////////////////////////
//
//等待接收数据,并返回第一个数据，超过delay_ms没有回应表示
//系统故障，跳出程序防止程序进入无穷的等待。
//
/////////////////////////////////////////////////
char RY_Ymodem::ACK(int delay_ms)
{
    int timeflag = 0;
    while(!MyComRevBUff.size())
    {
        delay_msec(1);//出让线程1ms
        timeflag++;
        if(timeflag>=delay_ms) return 0xFF;//delay_ms,没有反应，直接跳出
    }
    return GetByte();
}

/**
  * @brief  Calculate CRC16 checksum.
  * @param  [in] buff: The data to be calculated.
  * @param  [in] len:  The length of the data to be calculated.
  * @return Calculated CRC16 checksum.
  */
uint16_t RY_Ymodem::crc16(uint8_t *buff, uint32_t len)
{
  uint16_t crc = 0;

  while(len--)
  {
    crc ^= (uint16_t)(*(buff++)) << 8;

    for(int i = 0; i < 8; i++)
    {
      if(crc & 0x8000)
      {
        crc = (crc << 1) ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    }
  }

  return crc;
}

void RY_Ymodem::WriteCRC16(uint16_t size)
{
    uint16_t ResCRC16 = crc16(TxBuffer+3,size);

   TxBuffer[size + YMODEM_PACKET_OVERHEAD-2] = ResCRC16/256;
   TxBuffer[size + YMODEM_PACKET_OVERHEAD-1] = ResCRC16%256;
}

void RY_Ymodem::CPPtest(void)
{
    //ACK(500);
    qDebug("txlength = 0!\r\n");
    TxLength=2;
    return;
}
