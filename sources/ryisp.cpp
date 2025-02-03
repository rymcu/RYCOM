#include <ryisp.h>

DeviceInfo_t DeviceInfo;//赋值
unsigned int ISPBeginAddr;
char GetByte()
{
    //MyComRevBUff.size();
    char temp;
    temp = MyComRevBUff.at(0);
    MyComRevBUff.remove(0,1);
    return temp;
}
void SendByte(char SendData)
{
    MyCom.write(&SendData,1);//mycom.h声明了MyCom串口对象
}
/////////////////////////////////////////////////
//
//等待接收数据，第一个字节如果是0x79则认为ok返回1，如果是
//0x1f则认为失败返回0，持续检测500ms,超过500ms没有回应表示
//系统故障，跳出程序防止程序进入无穷的等待。
//
/////////////////////////////////////////////////
char ACK()    
{
    /*
  delay_msec(200);//延时200ms
         if(MyComRevBUff.size())//收到串口数据
        {
            //ISMyComRevData = 0;
            if(0x79 == GetByte()) return 1;
            else                  return 0;
        }
        return 0;
        */
    int timeflag = 0;
    while(!MyComRevBUff.size())
    {
        delay_msec(1);//出让线程
        timeflag++;
        if(timeflag>=500) return 0;//等待500ms,没有反应，直接跳出
    }
     //ISMyComRevData = 0;
    if(0x79 == GetByte()) return 1;
    else                  return 0;


}
/////////////////////////////////////////////////
//
//计算len个字节的checksum，也就是计算异或
//
/////////////////////////////////////////////////
static unsigned char CheckSum(unsigned char *data, int len)      
{
    int i;
    unsigned char cs;
    cs= 0;
    for ( i=0; i<len; i++ )
        cs ^= data[i];
    return cs;
}
/////////////////////////////////////////////////
//
//串口同步
//发送0x7F,接收到0x9F,表示串口同步成佛！
//
/////////////////////////////////////////////////
char StartISP()
{
    //发送握手指令
    SendByte(0x7F);
    //接收到0x9F返回1
    if(ACK()) return 1;
    else      return 0;
}
////////////////////////////////////////////////
//
//获取ISP命令
//
/////////////////////////////////////////////////
char GetCMD(DeviceInfo_t *DInfo)
{
   char error,len;

   //step1 发送指令0x00,0xFF,并等待ACK
   SendByte(0x00);
   SendByte(0xFF);
   error = ACK();if(!error) return 0;

   //step2 接收数据
   len = GetByte();  DInfo->cmd_count = len;//第一个字节为命令数量N
   DInfo->bootloaderversion = GetByte();//第二个字节为版本号
   for(int i =0;i<len;i++) DInfo->cmd[i] = GetByte();//获取CMD
   error = ACK();if(!error) return 0;

    //step3 处理数据，并打印版本号，命令等
    //打印数据，并清空串口缓存
    return 1;
}
/////////////////////////////////////////////////
//
//获取目标芯片ID
//
/////////////////////////////////////////////////
char GetID(DeviceInfo_t *DInfo)
{
    char len,error;

    //step1 发送序列0x02,0xfd，并等待ACK
    SendByte(0x02);
    SendByte(0xFD);
    error = ACK();if(!error) return 0;

    //step2 接收PID，并等待ACK
    len = GetByte();//第一个字节为ID长度N-1
    for(int i =0;i<=len;i++) DInfo->PID[i] = GetByte();//获取ID
    error = ACK();if(!error) return 0;

    return 1;
    //step3 清空接收缓存，并打印PID   
}
/////////////////////////////////////////////////
//
//读取数据
//与写数据步骤类似
//addr必须能被4整除，len发送数据长度-1，单次不能超过256B
/////////////////////////////////////////////////
char ReadMem(unsigned char *data, unsigned int addr, unsigned char len)
{
    unsigned char temp[4],error;      //保存addr的四个字节
    int i;
    temp[0] = ((addr>>24) & 0xFF);
    temp[1] = ((addr>>16) & 0xFF);
    temp[2] = ((addr>> 8) & 0xFF);
    temp[3] = ((addr    ) & 0xFF);
    //step1 发送序列0x11,0xEE，并等待ACK
    SendByte(0x11);
    SendByte(0xEE);
    error = ACK();if(!error) return 0;
    //step2 发送地址，先发高字节
    SendByte(temp[0]);
    SendByte(temp[1]);
    SendByte(temp[2]);
    SendByte(temp[3]);
    //step3 发送地址校验，并等待ACK
    SendByte(CheckSum(temp, 4));
    error = ACK();if(!error) return 0;
    //step4，发送len及校验，并等待ACK
    SendByte(len);
    SendByte(~len);
    error = ACK();if(!error) return 0;
    //step4 接收长度为len+1的数据
    for(i=0;i<=len;i++) data[i] = GetByte();
    
    return 1;
}

/////////////////////////////////////////////////
//
//写入数据块，从*data处，往stm32的addr处，写入len+1字节数据
//
/////////////////////////////////////////////////
char WriteMem(unsigned char *data, unsigned int addr, unsigned char len)
   
{
    unsigned char temp[4],error;     //保存addr的四个字节
    int i;
    temp[0] = ((addr>>24) & 0xFF);
    temp[1] = ((addr>>16) & 0xFF);
    temp[2] = ((addr>>8 ) & 0xFF);
    temp[3] = ((addr    ) & 0xFF);

    //step1 发送序列0x31,0xCE,并等待ACK
    SendByte(0x31);
    SendByte(0xCE);
    error = ACK();if(!error) return 0;
    //step2 发送地址，先发高字节
    SendByte(temp[0]);
    SendByte(temp[1]);
    SendByte(temp[2]);
    SendByte(temp[3]);
    //step3 发送地址校验，并等待ACK
    SendByte(CheckSum(temp, 4));
    error = ACK();if(!error) return 0;
    //step4 发送len
    SendByte(len);
    //step5 连续发送数据，最后字节为校验，并等待ACK
    for(i=0;i<=len;i++)
    {
        SendByte(data[i]);
    }
    SendByte(len ^ CheckSum(data, len+1));
    //delay_msec(3000);
    error = ACK();if(!error) return 0;

   //清空串口缓存，并延时1s.
    return 1;
}

/////////////////////////////////////////////////
//
//全片擦除,bootloader V3.0以下有效
// step1 发送序列0x43,0xbc,并等待ACK
// step2 发送序列0xff,0x00,并等待ACK
// step3 清空串口接收缓冲
/////////////////////////////////////////////////
char EraseAll()
{
   char error;
    SendByte(0x43);//
    SendByte(0xBC);
    error = ACK();if(!error) return 0;
    SendByte(0xFF);
    SendByte(0x00);
    //不同的产品，全片擦除的时间长短不一，500ms的时间不一定够，
    //因此不能用现成的ACK函数，需重写如下：
    //error = ACK();if(!error) return 0;
    while(!MyComRevBUff.size())
    {
        delay_msec(1);//出让线程
    }
    if(0x79 == GetByte()) return 1;
    else                  return 0;
}
/////////////////////////////////////////////////
//
//全片擦除,bootloader V3.0及以上有效
// step1 发送序列0x43,0xbc,并等待ACK
// step2 发送序列0xff,0x00,并等待ACK
// step3 清空串口接收缓冲
/////////////////////////////////////////////////
char ExtendedEraseAll()
{
   char error;
    SendByte(0x44);//
    SendByte(0xBB);
    error = ACK();if(!error) return 0;

    unsigned char EraseCMD[2];
    EraseCMD[0] = 0xFF;
    EraseCMD[1] = 0xFF;

    SendByte(0xFF);//写入地址0xFFFF
    SendByte(0xFF);

    SendByte(CheckSum(EraseCMD, 2));//双字节校验

    //不同的产品，全片擦除的时间长短不一，500ms的时间不一定够，
    //因此不能用现成的ACK函数，需重写如下：
    //error = ACK();if(!error) return 0;
    while(!MyComRevBUff.size())
    {
        delay_msec(1);//出让线程
    }
    if(0x79 == GetByte()) return 1;
    else                  return 0;
}
/////////////////////////////////////////////////
//
//跳转执行指令,下载完成后，跳转到RAM或内部FLASH执行
//
//
/////////////////////////////////////////////////
char CMDGo(unsigned int addr)
{
    unsigned char temp[4],error;     //保存addr的四个字节
    int i;
    temp[0] = ((addr>>24) & 0xFF);
    temp[1] = ((addr>>16) & 0xFF);
    temp[2] = ((addr>>8 ) & 0xFF);
    temp[3] = ((addr    ) & 0xFF);

    //step1 发送序列0x21,0xDE,并等待ACK
    SendByte(0x21);
    SendByte(0xDE);
    error = ACK();if(!error) return 0;
    //step2 发送地址，先发高字节
    SendByte(temp[0]);
    SendByte(temp[1]);
    SendByte(temp[2]);
    SendByte(temp[3]);
    //step3 发送地址校验，并等待ACK
    SendByte(CheckSum(temp, 4));
    error = ACK();if(!error) return 0;

    return 1;
}
/////////////////////////////////////////////////
//
//设置DTR，RTS
//
//
//
/////////////////////////////////////////////////
void SetDTR(bool DTR)
{
    //if(MyCom.isOpen())
        MyCom.setDataTerminalReady(DTR);
}
void SetRTS(bool RTS)
{
    //if(MyCom.isOpen())
        MyCom.setRequestToSend(RTS);
}
//end of file
