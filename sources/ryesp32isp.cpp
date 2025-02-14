#include <ryesp32isp.h>
#include <qdebug.h>
target_registers_t *s_reg = NULL;
target_chip_t s_target = ESP_UNKNOWN_CHIP;
uint8_t DELIMITER = 0xC0;
uint8_t C0_REPLACEMENT[2] = {0xDB, 0xDC};
uint8_t DB_REPLACEMENT[2] = {0xDB, 0xDD};

void Uart_write(uint8_t* data,size_t size)
{
    MyCom.write((char *)data,size);//mycom.h声明了MyCom串口对象
}
uint8_t Uart_read(QByteArray *data,command_t command)
{
    if(command != SYNC) //同步指令不需要等待
    {
        if(false == MyCom.waitForReadyRead(8000)) return 1;//8s还么收到数据，认为有问题//函数目的：擦除需要时间，8s应该足够了
        delay_msec(1);
    }
    else
    {
        delay_msec(10);//延时10ms,等待数据接收完成,同步指令返回数据较多
    }

    if(MyCom.bytesAvailable()>=MIN_RESP_DATA_SIZE)
    {
        *data = MyCom.readAll();
        //qDebug() << "data_uart0: " <<data->toHex();
        return 0;
    }
    return 1;
}
void Send_0xC0()
{
    MyCom.write((char *)&DELIMITER,1);
}
/***********************************************************
 * esp32进入bootloader模式
 * 对板子自动下载电路目标有效
 * 通过DTR，RTS控制RST，GPIO0进入串口下载模式
 ***********************************************************/
void esp32_enter_bootloader()
{
    SetDTR(false);
    SetRTS(true);
    delay_msec(500);
    SetDTR(true);
    SetRTS(false);
    delay_msec(500);

    MyCom.clear();//再次清空，防止延时期间收到数据干扰，qDebug() << "esp_boot OK";
}
/***********************************************************
 * 下载同步命令
 * 发送特殊数据完成与目标bootloader实现同步
 ***********************************************************/
esp_loader_error_t esp32_loader_sync_cmd()
{// dirction(0) command(1)  size(2-3) checksum(4-7)        data(8-n),（多字节小端模式）
 //   0x00      0x08        0x24 0x00  0x00 0x00 0x00 0x00 36Btye特殊字节
//同步指令数据包0x00 0x08
    sync_command_t sync_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SYNC,
            .size = CMD_SIZE(sync_cmd),
            .checksum = 0
        },
        .sync_sequence = {
            0x07, 0x07, 0x12, 0x20,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        }
    };

    send_cmd_config cmd_config = {
        .cmd = &sync_cmd,
        .cmd_size = sizeof(sync_cmd)
    };

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 读寄存器命令
 *
 ***********************************************************/
esp_loader_error_t loader_read_reg_cmd(uint32_t address, uint32_t *reg)
{
    read_reg_command_t read_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = READ_REG,
            .size = CMD_SIZE(read_cmd),
            .checksum = 0
        },
        .address = address,
    };

    send_cmd_config cmd_config = {
        .cmd = &read_cmd,
        .cmd_size = sizeof(read_cmd),
        //.reg_value = reg,
    };
    cmd_config.reg_value =reg;

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 读寄存器函数
 *
 ***********************************************************/
esp_loader_error_t esp_loader_read_register(uint32_t address, uint32_t *reg_value)
{
    return loader_read_reg_cmd(address, reg_value);
}
/***********************************************************
 * 检测目标芯片型号
 * 通过读目标寄存器（CHIP_DETECT_MAGIC_REG_ADDR），获取芯片型号
 * 也可以通过GET_SECURITY_INFO(0x14)命令获取，但不是所有芯片都支持
 ***********************************************************/
esp_loader_error_t esp32_loader_detect_chip(target_chip_t *target_chip, target_registers_t **target_data)
{
    uint32_t magic_value;
     if(ESP_LOADER_SUCCESS != esp_loader_read_register(CHIP_DETECT_MAGIC_REG_ADDR,  &magic_value))
         return ESP_LOADER_ERROR_INVALID_TARGET;
     qDebug() << "esp_loader_read_register OK";

    for (int chip = 0; chip < ESP_MAX_CHIP; chip++) {
        for (int index = 0; index < MAX_MAGIC_VALUES; index++) {
            if (magic_value == esp_target[chip].chip_magic_value[index]) {
                *target_chip = (target_chip_t)chip;
                *target_data = (target_registers_t *)&esp_target[chip];
                return ESP_LOADER_SUCCESS;
            }
        }
    }

    return ESP_LOADER_ERROR_INVALID_TARGET;
}
/***********************************************************
 * 更新bootloader串口波特率命令
 ***********************************************************/
esp_loader_error_t loader_change_baudrate_cmd(uint32_t new_baudrate, uint32_t old_baudrate)
{
    change_baudrate_command_t baudrate_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = CHANGE_BAUDRATE,
            .size = CMD_SIZE(baudrate_cmd),
            .checksum = 0
        },
        .new_baudrate = new_baudrate,
        .old_baudrate = old_baudrate
    };

     send_cmd_config cmd_config = {
        .cmd = &baudrate_cmd,
        .cmd_size = sizeof(baudrate_cmd),
    };

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 下载开始命令
 * 完成flash的擦除工作
 ***********************************************************/
esp_loader_error_t loader_flash_begin_cmd(uint32_t offset, uint32_t erase_size, uint32_t block_size,uint32_t blocks_to_write, bool encryption)
{
    flash_begin_command_t flash_begin_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_BEGIN,
            //.size = CMD_SIZE(flash_begin_cmd) - (encryption ? 0 : sizeof(uint32_t)),
            //for mac
            .size = (uint16_t)(CMD_SIZE(flash_begin_cmd) - (encryption ? 0 : sizeof(uint32_t))),
            .checksum = 0
        },
        .erase_size = erase_size,
        .packet_count = blocks_to_write,
        .packet_size = block_size,
        .offset = offset,
        .encrypted = 0
    };

    s_sequence_number = 0;

    send_cmd_config cmd_config = {
        .cmd = &flash_begin_cmd,
        .cmd_size = sizeof(flash_begin_cmd) - (encryption ? 0 : sizeof(uint32_t)),
    };

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 挂载spi flash命令
 ***********************************************************/
esp_loader_error_t loader_spi_attach_cmd(uint32_t config)
{
    spi_attach_command_t attach_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SPI_ATTACH,
            .size = CMD_SIZE(attach_cmd),
            .checksum = 0
        },
        .configuration = config,
        .zero = 0
    };

     send_cmd_config cmd_config = {
        .cmd = &attach_cmd,
        .cmd_size = sizeof(attach_cmd),
    };

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 挂载spi flash命令
 ***********************************************************/
esp_loader_error_t esp32_loader_spi_attach(uint32_t spi_config,target_chip_t s_target)
{
    if (s_target == ESP8266_CHIP)
    {
           return loader_flash_begin_cmd(0, 0, 0, 0, s_target);
    }
    else
    {
           return loader_spi_attach_cmd(spi_config);
    }

}
/***********************************************************
 * 设置spi flash大小
 ***********************************************************/
esp_loader_error_t loader_spi_parameters(uint32_t total_size)
{
    write_spi_command_t spi_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SPI_SET_PARAMS,
            .size = 24,
            .checksum = 0
        },
        .id = 0,
        .total_size = total_size,
        .block_size = 64 * 1024,
        .sector_size = 4 * 1024,
        .page_size = 0x100,
        .status_mask = 0xFFFF,
    };

     send_cmd_config cmd_config = {
        .cmd = &spi_cmd,
        .cmd_size = sizeof(spi_cmd),
    };

    return send_cmd(&cmd_config);
}
/***********************************************************
 * 通过串口复位目标芯片
 * 仅对配自动下载电路目标有效
 ***********************************************************/
void esp32_laoder_reset_target()
{
    SetDTR(false);//1
    SetRTS(true);//0
    delay_msec(500);
    SetDTR(false);
    SetRTS(false);//qDebug() << "esp_reset OK";

}
/***********************************************************
 * 按照SLIP协议发送数据
 * 数据格式：
 * 1.开头和结尾使用0xC0
 * 2.中间使用0xDB 0xDC替代0xC0
 * 3.中间使用0xDB 0xDC替代0xDB
 ***********************************************************/
esp_loader_error_t SLIP_send( uint8_t *data,size_t size)
{
    uint32_t to_write = 0; // Bytes ready to write as they are
    uint32_t written = 0;  // Bytes already written
    //test-begin
    //for (uint32_t i = 0; i < size; i++)
   // {
       // qDebug()<<"data:"<<QString::number(data[i], 16).toUpper();
    //}
    //test-end
    for (uint32_t i = 0; i < size; i++)
    {
        if (data[i] != 0xC0 && data[i] != 0xDB)
        {
            to_write++; // Queue this byte for writing
            continue;
        }
        // We have a byte that needs encoding, write the queue first
        if (to_write > 0)
        {
            Uart_write(&data[written], to_write);//发送不要编码的数据 Write the bytes that don't need encoding
        }
        // Write the encoded byte//需要编码发送：0xDB, 0xDC或0xDB, 0xDD
        if (data[i] == 0xC0)
        {
            Uart_write(C0_REPLACEMENT, 2);
        }
        else
        {
           Uart_write(DB_REPLACEMENT, 2);
        }
        // Update to start again after the encoded byte
        written = i + 1;
        to_write = 0;
    }
    // Write the rest of the bytes that didn't need encoding
    if (to_write > 0)
    {
        Uart_write(&data[written], to_write);
    }

    return ESP_LOADER_SUCCESS;
}
/***********************************************************
 * 接收SLIP数据包并还原数据
 * 数据格式：
 ***********************************************************/
esp_loader_error_t SLIP_receive_packet(uint8_t *buff, size_t *recv_size,command_t command)
{
    QByteArray data_uart;//读取全部数据
    if(Uart_read(&data_uart,command)) return ESP_LOADER_ERROR_INVALID_RESPONSE;//数据接收异常
    size_t data_size = data_uart.size();//串口接收数据大小
    //test
    //qDebug() << "data_uart: " <<data_uart.toHex();

    if(DELIMITER != (uint8_t)data_uart.at(0)) return ESP_LOADER_ERROR_INVALID_RESPONSE;//数据接收异常
    //qDebug() << "first 0xC0 ";
    //过滤开始连续发送2给0xC0的情况
    uint8_t flag_2_0xC0 = 0;
    if(DELIMITER != (uint8_t)data_uart.at(1))
    {
        buff[0] = (uint8_t)data_uart.at(1);
        flag_2_0xC0 = 0;
    }
    else
    {
         buff[0] = (uint8_t)data_uart.at(2);
         flag_2_0xC0 = 1;
    }
    //qDebug() << "buff[0]=0x:"<<QString::number(buff[0], 16).toUpper();
    uint8_t ch;
    size_t replace_times = 0;
    // Receive either until either delimiter or maximum receive size
    for (size_t i = 1; i < data_size; i++)//接收数据直到0xC0或接收数据达到最大长度
    {
       ch = (uint8_t)data_uart.at(1+flag_2_0xC0 + i+ replace_times);

        if (ch == 0xDB)//需要解码：0xDB, 0xDC或0xDB, 0xDD
        {
           ch = (uint8_t)data_uart.at(1+flag_2_0xC0 + i+ 1 + replace_times);
            if (ch == 0xDC)
            {
                buff[i] = 0xC0;
            }
            else if (ch == 0xDD)
            {
                buff[i] = 0xDB;
            }
            else
            {
                return ESP_LOADER_ERROR_INVALID_RESPONSE;
            }
            replace_times++;
            //qDebug() << "buff[i]=0x:"<<QString::number(buff[i], 16).toUpper();
        }
        else if (ch == DELIMITER)
        {
            *recv_size = i;
            //qDebug() << "uart data ok";
            return ESP_LOADER_SUCCESS;
        }
        else
        {
            buff[i] = ch;
            //qDebug() << "buff[i]=0x:"<<QString::number(buff[i], 16).toUpper();
        }
    }

    // Wait for delimiter if we already reached max receive size
    // This enables us to ignore unsupported or unecessary packet data instead of failing
    //接收最大长度后，等待0xC0
    return ESP_LOADER_ERROR_INVALID_RESPONSE;
}
/***********************************************************
 * 返回数据包确认
 * 确认：数据方向，指令类型，倒数3-4字节状态是否正确，并存储相应接收数据
 * 本代码使用ESP32 ROM模式，不使用stub模式，因此最后两个字节数据为保留字
 ***********************************************************/
esp_loader_error_t check_response(send_cmd_config *config)
{ //                        通用响应头（8B）      响应状态(2B)    响应最大数据64B（为什么）
    uint8_t buf[sizeof(common_response_t) + sizeof(response_status_t) + MAX_RESP_DATA_SIZE]; // 接收数据的缓存

    common_response_t *response = (common_response_t *)&buf[0];           // 获取通用响应头
    command_t command = (command_t)((command_common_t *)config->cmd)->command; // 获取命令类型

    // If the command has fixed response data size, require all of it to be received
    uint32_t minimum_packet_recv = sizeof(common_response_t) + sizeof(response_status_t); // 最小接收数据包大小为8+2=10B
    if (config->resp_data_recv_size == NULL)
    {
        minimum_packet_recv += config->resp_data_size;
    }

    size_t packet_recv = 0;
    do//while内的表达式为真时，执行循环体
    {
       if(ESP_LOADER_SUCCESS!=SLIP_receive_packet(buf,&packet_recv,command)) return ESP_LOADER_ERROR_INVALID_RESPONSE;
    } while ((response->direction != READ_DIRECTION) || (response->command != command) ||
             packet_recv < minimum_packet_recv);//接收到数据方向不为读，命令类型不匹配，数据包长度小于最小接收长度，则继续接收数据包
    if((response->direction != READ_DIRECTION) || (response->command != command) ||(packet_recv < minimum_packet_recv))
        return ESP_LOADER_ERROR_INVALID_RESPONSE;
     //qDebug() << "direction command packet_recv size OK!";
    response_status_t *status = (response_status_t *)&buf[packet_recv - sizeof(response_status_t)-2];//最后2个字节为状态字.ROM模式为倒数3-4字节

    if (status->failed)//状态为1，失败
    {
        //log_loader_internal_error(status->error);
        return ESP_LOADER_ERROR_INVALID_RESPONSE;
    }

    if (config->reg_value != NULL)
    {
        *config->reg_value = response->value;
    }

    if (config->resp_data != NULL)
    {
        const size_t resp_data_size = packet_recv - sizeof(common_response_t) - sizeof(response_status_t);

        memcpy(config->resp_data, &buf[sizeof(common_response_t)], resp_data_size);

        if (config->resp_data_recv_size != NULL)
        {
            *config->resp_data_recv_size = resp_data_size;
        }
    }
    //qDebug() << "status OK!";
    return ESP_LOADER_SUCCESS;
}
/***********************************************************
 * 发送命令数据包，并校验返回数据包的正确性
 ***********************************************************/
esp_loader_error_t send_cmd(send_cmd_config *config)
{
    MyCom.clear();//发送前清空串口
    esp_loader_error_t error = ESP_LOADER_SUCCESS;
    Send_0xC0();//串口发送0xC0
    SLIP_send((uint8_t*)config->cmd, config->cmd_size);
    if (config->data != NULL && config->data_size != 0)
    {
       SLIP_send((uint8_t *)config->data, config->data_size);
    }
    Send_0xC0();//串口发送0xC0

    //command_t command = (command_t)((command_common_t *)config->cmd)->command;
    //const uint8_t response_cnt = command == SYNC ? 8 : 1; // SYNC命令接收8个包，其他命令接收1个包
    const uint8_t response_cnt =1;
    for (uint8_t recv_cnt = 0; recv_cnt < response_cnt; recv_cnt++)
    {
       error = check_response(config);
    }

    return error;
}

bool encryption_in_begin_flash_cmd(const target_chip_t target)
{
    return esp_target[target].encryption_in_begin_flash_cmd;
}
static uint32_t calc_erase_size(const target_chip_t target, const uint32_t offset,
                                const uint32_t image_size)
{
    if (target != ESP8266_CHIP) {
        return image_size;
    } else {
        /* Needed to fix a bug in the ESP8266 ROM */
        const uint32_t sectors_per_block = 16U;
        const uint32_t sector_size = 4096U;

        const uint32_t num_sectors = (image_size + sector_size - 1) / sector_size;
        const uint32_t start_sector = offset / sector_size;

        uint32_t head_sectors = sectors_per_block - (start_sector % sectors_per_block);

        /* The ROM bug deletes extra num_sectors if we don't cross the block boundary
           and extra head_sectors if we do */
        if (num_sectors <= head_sectors) {
            return ((num_sectors + 1) / 2) * sector_size;
        } else {
            return (num_sectors - head_sectors) * sector_size;
        }
    }
}
esp_loader_error_t esp_loader_flash_start(uint32_t offset, uint32_t image_size, uint32_t block_size)
{
    s_flash_write_size = block_size;

    // Both the address and image size must be aligned to 4 bytes
    if (offset % 4 != 0 || image_size % 4 != 0) {
        return ESP_LOADER_ERROR_INVALID_PARAM;
    }

    bool encryption_in_cmd = encryption_in_begin_flash_cmd(s_target);
    const uint32_t erase_size = calc_erase_size(s_target, offset, image_size);
    const uint32_t blocks_to_write = (image_size + block_size - 1) / block_size;
    qDebug()<<"encryption_in_cmd:"<<encryption_in_cmd;
    qDebug()<<"s_target:"<<s_target;
    return loader_flash_begin_cmd(offset, erase_size, block_size, blocks_to_write, encryption_in_cmd);//擦除需要重新等待多长时间RYMCU
}
static uint8_t compute_checksum(const uint8_t *data, uint32_t size)
{
    uint8_t checksum = 0xEF;

    while (size--) {
        checksum ^= *data++;
    }

    return checksum;
}
esp_loader_error_t loader_flash_data_cmd(const uint8_t *data, uint32_t size)
{
    data_command_t data_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_DATA,
            //.size = CMD_SIZE(data_cmd) + size,
            //for mac
            .size = uint16_t(CMD_SIZE(data_cmd) + size),
            .checksum = compute_checksum(data, size)
        },
        .data_size = size,
        .sequence_number = s_sequence_number++,
    };

    send_cmd_config cmd_config = {
        .cmd = &data_cmd,
        .cmd_size = sizeof(data_cmd),
        .data = data,
        .data_size = size,
    };

    return send_cmd(&cmd_config);
}
esp_loader_error_t esp_loader_flash_write(void *payload, uint32_t size)
{
    uint32_t padding_bytes = s_flash_write_size - size;
    uint8_t *data = (uint8_t *)payload;
    uint32_t padding_index = size;

    if (size > s_flash_write_size) {
        return ESP_LOADER_ERROR_INVALID_PARAM;
    }

    const uint8_t padding_pattern = 0xFF;
    while (padding_bytes--) {
        data[padding_index++] = padding_pattern;
    }

   // unsigned int attempt = 0;
    esp_loader_error_t result = ESP_LOADER_ERROR_FAIL;
    //do {
        result = loader_flash_data_cmd(data, s_flash_write_size);
        //attempt++;
    //} while (result != ESP_LOADER_SUCCESS && attempt < SERIAL_FLASHER_WRITE_BLOCK_RETRIES);

    return result;
}


// For esp8266, esp32, esp32s2
#define BOOTLOADER_ADDRESS_V0       0x1000
// For esp32s3 and later chips
#define BOOTLOADER_ADDRESS_V1       0x0
#define PARTITION_ADDRESS           0x8000
#define APPLICATION_ADDRESS         0x10000

void get_esp32_addr(target_chip_t target, esp32_binaries_t *bins)
{
    switch (target)
    {
        case ESP8266_CHIP:
        case ESP32_CHIP:
        case ESP32S2_CHIP:
            bins->boot.addr = BOOTLOADER_ADDRESS_V0;
            break;
        default:
             bins->boot.addr = BOOTLOADER_ADDRESS_V1;
            break;
    }
    bins->part.addr = PARTITION_ADDRESS;
    bins->app.addr  = APPLICATION_ADDRESS;
}
esp_loader_error_t esp_loader_flash_detect_size(uint32_t *flash_size)
{
    typedef struct {
        uint8_t id;
        uint32_t size;
    } size_id_size_pair_t;

    /* There is no rule manufacturers have to follow for assigning these parts of the flash ID,
       these constants have been taken from esptool source code. */
    static const size_id_size_pair_t size_mapping[] = {
        { 0x12, 256 * 1024 },
        { 0x13, 512 * 1024 },
        { 0x14, 1 * 1024 * 1024 },
        { 0x15, 2 * 1024 * 1024 },
        { 0x16, 4 * 1024 * 1024 },
        { 0x17, 8 * 1024 * 1024 },
        { 0x18, 16 * 1024 * 1024 },
        { 0x19, 32 * 1024 * 1024 },
        { 0x1A, 64 * 1024 * 1024 },
        { 0x1B, 128 * 1024 * 1024 },
        { 0x1C, 256 * 1024 * 1024 },
        { 0x20, 64 * 1024 * 1024 },
        { 0x21, 128 * 1024 * 1024 },
        { 0x22, 256 * 1024 * 1024 },
        { 0x32, 256 * 1024 },
        { 0x33, 512 * 1024 },
        { 0x34, 1 * 1024 * 1024 },
        { 0x35, 2 * 1024 * 1024 },
        { 0x36, 4 * 1024 * 1024 },
        { 0x37, 8 * 1024 * 1024 },
        { 0x38, 16 * 1024 * 1024 },
        { 0x39, 32 * 1024 * 1024 },
        { 0x3A, 64 * 1024 * 1024 },
    };

    uint32_t flash_id = 0;
    qDebug()<<"1.before spi_flash_command";
    RETURN_ON_ERROR(spi_flash_command(SPI_FLASH_READ_ID, NULL, 0, &flash_id, 24) );
    uint8_t size_id = flash_id >> 16;
    qDebug()<<"after spi_flash_command";
    // Try finding the size id within supported size ids
    for (size_t i = 0; i < sizeof(size_mapping) / sizeof(size_mapping[0]); i++) {
        if (size_id == size_mapping[i].id) {
            *flash_size = size_mapping[i].size;
            return ESP_LOADER_SUCCESS;
        }
    }

    return ESP_LOADER_ERROR_UNSUPPORTED_CHIP;
}
esp_loader_error_t loader_write_reg_cmd(uint32_t address, uint32_t value,
                                        uint32_t mask, uint32_t delay_us)
{
    write_reg_command_t write_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = WRITE_REG,
            .size = CMD_SIZE(write_cmd),
            .checksum = 0
        },
        .address = address,
        .value = value,
        .mask = mask,
        .delay_us = delay_us
    };

        send_cmd_config cmd_config = {
        .cmd = &write_cmd,
        .cmd_size = sizeof(write_cmd),
    };

    return send_cmd(&cmd_config);
}
esp_loader_error_t esp_loader_write_register(uint32_t address, uint32_t reg_value)
{
    return loader_write_reg_cmd(address, reg_value, 0xFFFFFFFF, 0);
}
static esp_loader_error_t spi_set_data_lengths(size_t mosi_bits, size_t miso_bits)
{
    if (mosi_bits > 0) {
        RETURN_ON_ERROR( esp_loader_write_register(s_reg->mosi_dlen, mosi_bits - 1) );
    }
    if (miso_bits > 0) {
        RETURN_ON_ERROR( esp_loader_write_register(s_reg->miso_dlen, miso_bits - 1) );
    }

    return ESP_LOADER_SUCCESS;
}
static esp_loader_error_t spi_set_data_lengths_8266(size_t mosi_bits, size_t miso_bits)
{
    uint32_t mosi_mask = (mosi_bits == 0) ? 0 : mosi_bits - 1;
    uint32_t miso_mask = (miso_bits == 0) ? 0 : miso_bits - 1;
    return esp_loader_write_register(s_reg->usr1, (miso_mask << 8) | (mosi_mask << 17));
}
static esp_loader_error_t spi_flash_command(spi_flash_cmd_t cmd, void *data_tx, size_t tx_size, void *data_rx, size_t rx_size)
{
    //assert(rx_size <= 32); // Reading more than 32 bits back from a SPI flash operation is unsupported
    //assert(tx_size <= 64); // Writing more than 64 bytes of data with one SPI command is unsupported
if((rx_size >32) || (tx_size >64)) return ESP_LOADER_ERROR_FAIL;
    uint32_t SPI_USR_CMD  = (1 << 31);
    uint32_t SPI_USR_MISO = (1 << 28);
    uint32_t SPI_USR_MOSI = (1 << 27);
    uint32_t SPI_CMD_USR  = (1 << 18);
    uint32_t CMD_LEN_SHIFT = 28;

    // Save SPI configuration
    uint32_t old_spi_usr;
    uint32_t old_spi_usr2;
    qDebug()<<"2.before esp_loader_read_register";
    RETURN_ON_ERROR( esp_loader_read_register(s_reg->usr, &old_spi_usr) );
    RETURN_ON_ERROR( esp_loader_read_register(s_reg->usr2, &old_spi_usr2) );
    qDebug()<<"3.after esp_loader_read_register";

    if (s_target == ESP8266_CHIP) {
        RETURN_ON_ERROR( spi_set_data_lengths_8266(tx_size, rx_size) );
    } else {
        RETURN_ON_ERROR( spi_set_data_lengths(tx_size, rx_size) );
    }

    uint32_t usr_reg_2 = (7 << CMD_LEN_SHIFT) | cmd;
    uint32_t usr_reg = SPI_USR_CMD;
    if (rx_size > 0) {
        usr_reg |= SPI_USR_MISO;
    }
    if (tx_size > 0) {
        usr_reg |= SPI_USR_MOSI;
    }

    RETURN_ON_ERROR( esp_loader_write_register(s_reg->usr, usr_reg) );
    RETURN_ON_ERROR( esp_loader_write_register(s_reg->usr2, usr_reg_2 ) );

    if (tx_size == 0) {
        // clear data register before we read it
        RETURN_ON_ERROR( esp_loader_write_register(s_reg->w0, 0) );
    } else {
        uint32_t *data = (uint32_t *)data_tx;
        uint32_t words_to_write = (tx_size + 31) / (8 * 4);
        uint32_t data_reg_addr = s_reg->w0;

        while (words_to_write--) {
            uint32_t word = *data++;
            RETURN_ON_ERROR( esp_loader_write_register(data_reg_addr, word) );
            data_reg_addr += 4;
        }
    }

    RETURN_ON_ERROR( esp_loader_write_register(s_reg->cmd, SPI_CMD_USR) );

    uint32_t trials = 10;
    while (trials--) {
        uint32_t cmd_reg;
        RETURN_ON_ERROR( esp_loader_read_register(s_reg->cmd, &cmd_reg) );
        if ((cmd_reg & SPI_CMD_USR) == 0) {
            break;
        }
    }

    if (trials == 0) {
        return ESP_LOADER_ERROR_TIMEOUT;
    }

    RETURN_ON_ERROR( esp_loader_read_register(s_reg->w0, (uint32_t *)data_rx) );

    // Restore SPI configuration
    RETURN_ON_ERROR( esp_loader_write_register(s_reg->usr, old_spi_usr) );
    RETURN_ON_ERROR( esp_loader_write_register(s_reg->usr2, old_spi_usr2) );

    return ESP_LOADER_SUCCESS;
}
//end of file
