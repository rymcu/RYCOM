#include "HexToBin.h"
#include <stdio.h>
#include <string.h>

QByteArray text;

/********************************************************************************
input:
	c:单个字符('0'~'9' 'a'~'f', 'A'~'F')
output:
	单个字符转化为单个字符
********************************************************************************/
static uint8_t HexCharToBinBinChar(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'z')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'Z')
		return c - 'A' + 10;
	return 0xff;
}
 
/********************************************************************************
input:
	p: 两个文本字符
output:
	转化为1个字节
********************************************************************************/
static uint8_t Hex2Bin(const char *p)
{
	uint8_t tmp = 0;
	tmp = HexCharToBinBinChar(p[0]);
	tmp <<= 4;
	tmp |= HexCharToBinBinChar(p[1]);
	return tmp;
}
 
/********************************************************************************
input:
	src: hex单行字符串
	p->type: 如果函数返回结果正确，这里就存着转化后的类型
	p->len: 如果函数运行正确，这里就存着转化后的bin数据长度
	p->data: 如果函数运行正确，长度并且不为0，该指针就只想转化后的数据
	p->addr[0]: 如果函数返回结果正确，这里就存着地址的低字节
	p->addr[1]: 如果函数返回结果正确，这里就存着地址的低字节
output:
	返回hex格式流分析的结果
********************************************************************************/
static RESULT_STATUS HexFormatUncode(const char *src, BinFarmat *p)
{
	uint8_t check = 0, tmp[4], binLen;
	uint16_t hexLen = strlen(src);
	uint16_t num = 0, offset = 0;
	if (hexLen > HEX_MAX_LENGTH)		//数据内容过长
		return RES_DATA_TOO_LONG;
	if (hexLen < HEX_MIN_LEN)
		return RES_DATA_TOO_SHORT;	//数据内容过短
	if (src[0] != ':')
		return RES_NO_COLON;		//没有冒号
	if ((hexLen - 1) % 2 != 0)
		return RES_LENGTH_ERROR;	//hexLen的长度应该为奇数
	binLen = (hexLen - 1) / 2;		//bin总数据的长度，包括长度，地址，类型校验等内容
	while (num < 4)
	{
		offset = (num << 1) + 1;
		tmp[num] = Hex2Bin(src + offset);
		check += tmp[num];
		num++;
	}
	p->len = tmp[0];			//把解析的这些数据保存到结构体中
	p->addr = tmp[1];
	p->addr <<= 8;
	p->addr += tmp[2];
	p->type = tmp[3];
	while (num < binLen)
	{
		offset = (num << 1) + 1;        //保存真正的bin格式数据流
		p->data[num - 4] = Hex2Bin(src + offset);
		check += p->data[num - 4];		
		num++;
	}
	if (p->len != binLen - 5)		//检查hex格式流的长度和数据的长度是否一致
		return RES_LENGTH_ERROR;
	if (check != 0)				//检查校验是否通过
		return RES_CHECK_ERROR;
	return RES_OK;
}
 
int  insertsomething(QByteArray* desQByte,uint8_t* addr, int num )
{
    QByteArray str="a";

    for(int i=0;i<num;i++)
    {
        str[0] = *(addr+i);
        desQByte->append(str);
    }
    return num;
}

RESULT_STATUS HexFile2BinFile(const char *src, char *dest, uint32_t *addr)
{  
    FILE *src_file;// *dest_file;
	uint16_t addr_low = 0;
	uint32_t addr_hign = 0;
	char buffer_hex[600];
	uint8_t buffer_bin[255];
	BinFarmat gBinFor;
	RESULT_STATUS res;

     //QByteArray tset;

	gBinFor.data = buffer_bin;
	src_file = fopen(src, "r");		//以文本的形式打开一个hex文件
	if (!src_file)
		return RES_HEX_FILE_NOT_EXIST;
    //dest_file = fopen(dest, "wb");		//以二进制写的方式打开文件，文件不存在也没影响
    //if (!dest_file)
        //return RES_BIN_FILE_PATH_ERROR;
	fseek(src_file, 0, SEEK_SET);           //定位到开头，准备开始读取数据  
    char beginflag = 1;
    char secondline = 1;
	while (!feof(src_file))
	{
		fscanf(src_file, "%s\r\n", buffer_hex);




        //get the begin addr
        unsigned int Low,High,addr_temp = 0;

        if(buffer_hex[2] == '2')
        {

            for(char i=0;i<4;i++)
            {
                 if(buffer_hex[6-i]>='0' && buffer_hex[6-i]<='9')
                 {
                    Low = buffer_hex[6-i]-'0';
                 }
                 else if(buffer_hex[6-i]>='a' && buffer_hex[6-i]<='f')
                 {
                    Low = buffer_hex[6-i]-'a'+10;
                 }
                 else if(buffer_hex[6-i]>='A' && buffer_hex[6-i]<='F')
                 {
                    Low = buffer_hex[6-i]-'A'+10;
                 }
                 else
                 {
                     Low = 0xFF;
                 }

                Low = Low<<(4*i);

                if(buffer_hex[12-i]>='0' && buffer_hex[12-i]<='9')
                {
                   High = buffer_hex[12-i]-'0';
                }
                else if(buffer_hex[12-i]='a' && buffer_hex[12-i]<='f')
                {
                   High = buffer_hex[12-i]-'a'+10;
                }
                else if(buffer_hex[12-i]>='A' && buffer_hex[12-i]<='F')
                {
                   High = buffer_hex[12-i]-'A'+10;
                }
                else
                {
                   High = 0xFF;
                }

               High = High<<(4*i+16);

                addr_temp = addr_temp + Low + High;
            }
            //addr_temp= 0x08000000;
            if(beginflag)
            {
              *addr = addr_temp;//only frist time！
                beginflag = 0;
            }

        }
        //get addr offset
         if(buffer_hex[1] == '1')
         {
             Low =0;
             for(char i=0;i<4;i++)
             {
                  if(buffer_hex[6-i]>='0' && buffer_hex[6-i]<='9')
                  {
                     Low = buffer_hex[6-i]-'0';
                  }
                  else if(buffer_hex[6-i]>='a' && buffer_hex[6-i]<='f')
                  {
                     Low = buffer_hex[6-i]-'a'+10;
                  }
                  else if(buffer_hex[6-i]>='A' && buffer_hex[6-i]<='F')
                  {
                     Low = buffer_hex[6-i]-'A'+10;
                  }
                  else
                  {
                      Low = 0xFF;
                  }

                 Low = Low<<(4*i);
             }
             //addr_temp= 0x08000000;
             if(secondline)
             {
               *addr = *addr+Low;//only frist time！
                 secondline = 0;
             }

         }


        //




		res = HexFormatUncode(buffer_hex, &gBinFor);
		if (res != RES_OK)
		{
			fclose(src_file);
            //fclose(dest_file);
			return res;
		}
		switch (gBinFor.type)
		{
			case 0:			//数据记录
				addr_low = gBinFor.addr;
                                //数据指针偏移
               // fseek(dest_file, addr_low + addr_hign, SEEK_SET);


               insertsomething(&text,(uint8_t*)gBinFor.data, gBinFor.len);

                /*if (fwrite((const uint8_t*)gBinFor.data, gBinFor.len, 1, dest_file) != 1)
				{
					fclose(src_file);
					fclose(dest_file);
					return RES_WRITE_ERROR;
                }*/
				break;
			case 1:		        //数据结束
				fclose(src_file);
                //fclose(dest_file);
				return RES_OK;
			case 2:
				addr_hign = ((uint32_t)gBinFor.addr) << 2;
				break;
			case 4:			//线性段地址
				addr_hign = ((uint32_t)gBinFor.addr) << 16;
				break;
			default:
				fclose(src_file);
                //fclose(dest_file);
				return RES_TYPE_ERROR;
		}
	}
	fclose(src_file);
    //fclose(dest_file);
	return RES_HEX_FILE_NO_END;
}

