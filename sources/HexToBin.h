#ifndef HEX2BIN_H
#define HEX2BIN_H
 
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
 #include <QString>

extern QByteArray text;

#define HEX_MAX_LENGTH		521
#define HEX_MIN_LEN		11
 
typedef enum {
	RES_OK = 0,		//正确
	RES_DATA_TOO_LONG,	//数据太长
	RES_DATA_TOO_SHORT,	//数据太短
	RES_NO_COLON,		//无标号
	RES_TYPE_ERROR,		//类型出错，或许不存在
	RES_LENGTH_ERROR,	//数据长度字段和内容的总长度不对应
	RES_CHECK_ERROR,	//校验错误
	RES_HEX_FILE_NOT_EXIST,	//HEX文件不存在
	RES_BIN_FILE_PATH_ERROR,//BIN文件路径可能不正确
	RES_WRITE_ERROR,	//写数据出错
	RES_HEX_FILE_NO_END	//hex文件没有结束符
} RESULT_STATUS;
 
typedef struct {
	uint8_t len;
	uint8_t	type;
	uint16_t addr;
	uint8_t *data;
} BinFarmat;
 
RESULT_STATUS HexFile2BinFile(char *src, char *dest,uint32_t* addr);
#endif
