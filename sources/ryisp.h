#ifndef __RYISP_H__
#define __RYISP_H__

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
*  实现串口ISP底层驱动
*                                                              	
************************************************************/
#include <mycom.h>

typedef struct {
    char bootloaderversion;    //1字节，bootloader版本
    char cmd_count;            //支持的所有指令个数
    char cmd[16];              //支持的所有指令
    char PID[4];                   //从机ID
} DeviceInfo_t;

extern DeviceInfo_t DeviceInfo;//存储PID，版本，所支持指令等内容

extern unsigned int ISPBeginAddr;//存储程序编程首地址

void SendByte(char SendData);//串口发送一个字节
void Uart_write(uint8_t* data,size_t size);//串口发送多个字节
char GetByte();//从串口接收一个字节
char ACK();//接收到从机应答0x79,返回1
char StartISP();//主机和从机握手
char GetCMD(DeviceInfo_t *DInfo);//获取从机指令
char GetID(DeviceInfo_t *DInfo);//获取从机ID

//读从机数据，从地址addr处读取len+1字节数据，并存储与data中
char  ReadMem(unsigned char *data, unsigned int addr, unsigned char len);
//往从机写数据，从data中读取len+1字节数据，写入从机地址开始地址addr处
char WriteMem(unsigned char *data, unsigned int addr, unsigned char len);
//擦除从机全部存储器，bootloader V3.0以下有效
char EraseAll();
//擦除从机全部存储器,bootloader V3.0及以上有效
char ExtendedEraseAll();
//跳转执行指令,下载完成后，跳转到RAM或内部FLASH执行
char CMDGo(unsigned int addr);
//设置DTR，RTS
void SetDTR(bool DTR);
void SetRTS(bool RTS);

#endif
