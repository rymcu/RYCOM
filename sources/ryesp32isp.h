#ifndef __RYESP32ISP_H__
#define __RYESP32ISP_H__

/************************************************************
*             ESP32串口ISP驱动
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
#include <ryisp.h>
#include "esp32/esp32_protocol.h"
#include <qdebug.h>

//#define QT_DEBUG  //release版本注销，防止调试信息占用资源

#ifdef QT_DEBUG
#define myDebug() qDebug()
#else
#define myDebug() QNoDebug()
#endif

extern target_registers_t *s_reg;
extern target_chip_t s_target;//目标芯片型号
extern uint32_t ESP32_loader_daud;//临时存储ESP32下载波特率，用于改变波特率CMD

static uint32_t s_flash_write_size = 0;
static uint32_t s_sequence_number = 0;
static uint32_t s_target_flash_size = 0;

static  esp32_binaries_t esp32_bin ={NULL};//目标.bin文件信息

#define DEFAULT_FLASH_SIZE  16*1024*1024

//esp_loader_error_t flash_binary(const uint8_t *bin, size_t size, size_t address);
esp_loader_error_t esp_loader_flash_start(uint32_t offset, uint32_t image_size, uint32_t block_size);
esp_loader_error_t esp_loader_flash_write(void *payload, uint32_t size);
void esp32_laoder_reset_target();

esp_loader_error_t esp32_loader_sync_cmd();
esp_loader_error_t loader_change_baudrate_cmd(uint32_t new_baudrate, uint32_t old_baudrate);
esp_loader_error_t esp32_loader_detect_chip(target_chip_t *target_chip, target_registers_t **target_data);
void esp32_enter_bootloader();

esp_loader_error_t send_cmd(send_cmd_config *config);
esp_loader_error_t esp32_loader_spi_attach(uint32_t spi_config,target_chip_t s_target);
esp_loader_error_t loader_spi_parameters(uint32_t total_size);
esp_loader_error_t esp_loader_flash_detect_size(uint32_t *flash_size);
static esp_loader_error_t spi_flash_command(spi_flash_cmd_t cmd, void *data_tx, size_t tx_size, void *data_rx, size_t rx_size);

void get_esp32_addr(target_chip_t target, esp32_binaries_t *bins);

#endif
