#ifndef __ESP32_PROTOCOL_H__
#define __ESP32_PROTOCOL_H__

#include"stdio.h"

#define STATUS_FAILURE  1
#define STATUS_SUCCESS  0

#define READ_DIRECTION  1
#define WRITE_DIRECTION 0

#define CMD_SIZE(cmd) ( sizeof(cmd) - sizeof(command_common_t) )

// Maximum block sized for RAM and Flash writes, respectively.
#define ESP_RAM_BLOCK 0x1800

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef ROUNDUP
#define ROUNDUP(a, b) ((((unsigned)a + (unsigned)b - 1) / (unsigned)b) * (unsigned)b)
#endif

#define MAX_RESP_DATA_SIZE 64
#define READ_FLASH_ROM_DATA_SIZE 64

#define MIN_RESP_DATA_SIZE 12 //sizeof(common_response_t) + sizeof(response_status_t) + 2*0xC0=8+2+2B

#define RETURN_ON_ERROR(x) do {         \
    esp_loader_error_t _err_ = (x);     \
    if (_err_ != ESP_LOADER_SUCCESS) {  \
        return _err_;                   \
    }                                   \
} while(0)

/**
 * @brief Error codes
 */
typedef enum {
    ESP_LOADER_SUCCESS,                /*!< Success */
    ESP_LOADER_ERROR_FAIL,             /*!< Unspecified error */
    ESP_LOADER_ERROR_TIMEOUT,          /*!< Timeout elapsed */
    ESP_LOADER_ERROR_IMAGE_SIZE,       /*!< Image size to flash is larger than flash size */
    ESP_LOADER_ERROR_INVALID_MD5,      /*!< Computed and received MD5 does not match */
    ESP_LOADER_ERROR_INVALID_PARAM,    /*!< Invalid parameter passed to function */
    ESP_LOADER_ERROR_INVALID_TARGET,   /*!< Connected target is invalid */
    ESP_LOADER_ERROR_UNSUPPORTED_CHIP, /*!< Attached chip is not supported */
    ESP_LOADER_ERROR_UNSUPPORTED_FUNC, /*!< Function is not supported on attached target */
    ESP_LOADER_ERROR_INVALID_RESPONSE  /*!< Internal error */
} esp_loader_error_t;

/**
 * @brief Supported targets
 */
typedef enum {
    ESP8266_CHIP = 0,
    ESP32_CHIP   = 1,
    ESP32S2_CHIP = 2,
    ESP32C3_CHIP = 3,
    ESP32S3_CHIP = 4,
    ESP32C2_CHIP = 5,
    ESP32_RESERVED0_CHIP = 6, // Reserved for future use
    ESP32H2_CHIP = 7,
    ESP32C6_CHIP = 8,
    ESP_MAX_CHIP = 9,
    ESP_UNKNOWN_CHIP = 9
} target_chip_t;

typedef enum
{
    FLASH_BEGIN = 0x02,
    FLASH_DATA = 0x03,
    FLASH_END = 0x04,
    MEM_BEGIN = 0x05,
    MEM_END = 0x06,
    MEM_DATA = 0x07,
    SYNC = 0x08,
    WRITE_REG = 0x09,
    READ_REG = 0x0a,
    SPI_SET_PARAMS = 0x0b,
    SPI_ATTACH = 0x0d,
    READ_FLASH_ROM = 0x0e,
    CHANGE_BAUDRATE = 0x0f,
    FLASH_DEFL_BEGIN = 0x10,
    FLASH_DEFL_DATA = 0x11,
    FLASH_DEFL_END = 0x12,
    SPI_FLASH_MD5 = 0x13,
    GET_SECURITY_INFO = 0x14,
    READ_FLASH_STUB = 0xd2,
} command_t;

typedef enum
{
    RESPONSE_OK     = 0x00,
    INVALID_COMMAND = 0x05, // parameters or length field is invalid
    COMMAND_FAILED  = 0x06, // Failed to act on received message
    INVALID_CRC     = 0x07, // Invalid CRC in message
    FLASH_WRITE_ERR = 0x08, // After writing a block of data to flash, the ROM loader reads the value back and the 8-bit CRC is compared to the data read from flash. If they don't match, this error is returned.
    FLASH_READ_ERR  = 0x09, // SPI read failed
    READ_LENGTH_ERR = 0x0a, // SPI read request length is too long
    DEFLATE_ERROR   = 0x0b, // ESP32 compressed uploads only
} error_code_t;

typedef struct
{
    uint8_t direction;
    uint8_t command;    // One of command_t
    uint16_t size;
    uint32_t checksum;
} command_common_t;

typedef struct
{
    command_common_t common;
    uint32_t erase_size;
    uint32_t packet_count;
    uint32_t packet_size;
    uint32_t offset;
    uint32_t encrypted;
} flash_begin_command_t;

typedef struct
{
    command_common_t common;
    uint32_t data_size;
    uint32_t sequence_number;
    uint32_t zero_0;
    uint32_t zero_1;
} data_command_t;
typedef struct __attribute__((packed))
{
    command_common_t common;
    uint32_t new_baudrate;
    uint32_t old_baudrate;
} change_baudrate_command_t;
typedef struct __attribute__((packed))
{
    command_common_t common;
    uint32_t address;
} read_reg_command_t;
typedef struct __attribute__((packed))
{
    command_common_t common;
    uint32_t configuration;
    uint32_t zero; // ESP32 ROM only
} spi_attach_command_t;
typedef struct
{
    command_common_t common;
    uint32_t stay_in_loader;
} flash_end_command_t;
typedef struct __attribute__((packed))
{
    uint8_t direction;
    uint8_t command;    // One of command_t
    uint16_t size;
    uint32_t value;
} common_response_t;

typedef struct __attribute__((packed))
{
    uint8_t failed;
    uint8_t error;
} response_status_t;
typedef struct {
    void *cmd;
    size_t cmd_size;
    const void *data; // Set to NULL if the command has no data
    size_t data_size;
    void *resp_data; // Set to NULL if the response has no data
    size_t resp_data_size;
    uint32_t *resp_data_recv_size; /* Out parameter indicating actual size of the response read
                                      for commands where response size can vary, in which
                                      case resp_data_size is the maximum response data size allowed.
                                      Set to NULL to require fixed response size of resp_data_size. */
    uint32_t *reg_value; // Out parameter for the READ_REG command, will return zero otherwise
} send_cmd_config;

typedef struct
{
    command_common_t common;
    uint8_t sync_sequence[36];
} sync_command_t;

typedef struct __attribute__((packed))
{
    command_common_t common;
    uint32_t id;
    uint32_t total_size;
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t status_mask;
} write_spi_command_t;

typedef struct {
    uint32_t cmd;
    uint32_t usr;
    uint32_t usr1;
    uint32_t usr2;
    uint32_t w0;
    uint32_t mosi_dlen;
    uint32_t miso_dlen;
} target_registers_t;

#define MAX_MAGIC_VALUES 4
typedef esp_loader_error_t (*read_spi_config_t)(uint32_t efuse_base, uint32_t *spi_config);
typedef struct {
    target_registers_t regs;
    uint32_t efuse_base;
    uint32_t chip_magic_value[MAX_MAGIC_VALUES];
    uint32_t mac_efuse_offset;
    uint32_t chip_id;
    read_spi_config_t read_spi_config;
    bool encryption_in_begin_flash_cmd;
} esp_target_t;

// This ROM address has a different value on each chip model
#define CHIP_DETECT_MAGIC_REG_ADDR 0x40001000

#define ESP8266_SPI_REG_BASE 0x60000200
#define ESP32S2_SPI_REG_BASE 0x3f402000
#define ESP32H2_SPI_REG_BASE 0x60003000
#define ESP32C6_SPI_REG_BASE 0x60003000
#define ESP32xx_SPI_REG_BASE 0x60002000
#define ESP32_SPI_REG_BASE   0x3ff42000

#define CHIP_ID_NONE 0xFF

/**
 * Macro which can be used to check the error code,
 * and return in case the code is not ESP_LOADER_SUCCESS.
 */
#define RETURN_ON_ERROR(x) do {         \
    esp_loader_error_t _err_ = (x);     \
    if (_err_ != ESP_LOADER_SUCCESS) {  \
        return _err_;                   \
    }                                   \
} while(0)

static esp_loader_error_t spi_config_esp32(uint32_t efuse_base, uint32_t *spi_config)
{
    (void)(efuse_base);

    *spi_config = 0;
    return ESP_LOADER_SUCCESS;
}

// Applies for esp32s2, esp32c3 and esp32c3
static esp_loader_error_t spi_config_esp32xx(uint32_t efuse_base, uint32_t *spi_config)
{
    (void)(efuse_base);

    *spi_config = 0;
    return ESP_LOADER_SUCCESS;
}

// Some newer chips like the esp32c6 do not support configurable SPI
static esp_loader_error_t spi_config_unsupported(uint32_t efuse_base, uint32_t *spi_config)
{
    (void)(efuse_base);

    *spi_config = 0;
    return ESP_LOADER_SUCCESS;
}
static const esp_target_t esp_target[ESP_MAX_CHIP] = {

    // ESP8266
    {
         {
             ESP8266_SPI_REG_BASE + 0x00,
             ESP8266_SPI_REG_BASE + 0x1c,
             ESP8266_SPI_REG_BASE + 0x20,
             ESP8266_SPI_REG_BASE + 0x24,
             ESP8266_SPI_REG_BASE + 0x40,
             0,
             0,
        },
         0,            // Not used
         { 0xfff0c101, 0, 0, 0 },
         0, // Not used
         CHIP_ID_NONE,
        NULL,    // Not used
        false,
    },

    // ESP32
    {
         {
             ESP32_SPI_REG_BASE + 0x00,
             ESP32_SPI_REG_BASE + 0x1c,
             ESP32_SPI_REG_BASE + 0x20,
             ESP32_SPI_REG_BASE + 0x24,
             ESP32_SPI_REG_BASE + 0x80,
             ESP32_SPI_REG_BASE + 0x28,
             ESP32_SPI_REG_BASE + 0x2c,
        },
         0x3ff5A000,
         { 0x00f01d83, 0, 0, 0 },
         0x04,
         0,
        spi_config_esp32,
        false,
    },

    // ESP32S2
    {
         {
             ESP32S2_SPI_REG_BASE + 0x00,
             ESP32S2_SPI_REG_BASE + 0x18,
             ESP32S2_SPI_REG_BASE + 0x1c,
             ESP32S2_SPI_REG_BASE + 0x20,
             ESP32S2_SPI_REG_BASE + 0x58,
             ESP32S2_SPI_REG_BASE + 0x24,
             ESP32S2_SPI_REG_BASE + 0x28,
        },
         0x3f41A000,
         { 0x000007c6, 0, 0, 0},
         0x44,
         2,
         spi_config_esp32xx,
        true,
    },

    // ESP32C3
    {
         {
             ESP32xx_SPI_REG_BASE + 0x00,
             ESP32xx_SPI_REG_BASE + 0x18,
             ESP32xx_SPI_REG_BASE + 0x1c,
             ESP32xx_SPI_REG_BASE + 0x20,
             ESP32xx_SPI_REG_BASE + 0x58,
             ESP32xx_SPI_REG_BASE + 0x24,
             ESP32xx_SPI_REG_BASE + 0x28,
        },
         0x60008800,
         { 0x6921506f, 0x1b31506f, 0x4881606F, 0x4361606F },
         0x44,
         5,
        spi_config_esp32xx,
        true,
    },

    // ESP32S3
    {
         {
             ESP32xx_SPI_REG_BASE + 0x00,
             ESP32xx_SPI_REG_BASE + 0x18,
             ESP32xx_SPI_REG_BASE + 0x1c,
             ESP32xx_SPI_REG_BASE + 0x20,
             ESP32xx_SPI_REG_BASE + 0x58,
             ESP32xx_SPI_REG_BASE + 0x24,
             ESP32xx_SPI_REG_BASE + 0x28,
        },
         0x60007000,
         { 0x00000009, 0, 0, 0 },
         0x44,
         9,
        spi_config_esp32xx,
        true,
    },

    // ESP32C2
    {
         {
             ESP32xx_SPI_REG_BASE + 0x00,
             ESP32xx_SPI_REG_BASE + 0x18,
             ESP32xx_SPI_REG_BASE + 0x1c,
             ESP32xx_SPI_REG_BASE + 0x20,
             ESP32xx_SPI_REG_BASE + 0x58,
             ESP32xx_SPI_REG_BASE + 0x24,
             ESP32xx_SPI_REG_BASE + 0x28,
        },
         0x60008800,
         { 0x6f51306f, 0x7c41a06f, 0, 0 },
         0x40,
         12,
        spi_config_unsupported,
        true,
    },

    // ESP32H2
    {
         {
             ESP32H2_SPI_REG_BASE + 0x00,
             ESP32H2_SPI_REG_BASE + 0x18,
             ESP32H2_SPI_REG_BASE + 0x1c,
             ESP32H2_SPI_REG_BASE + 0x20,
             ESP32H2_SPI_REG_BASE + 0x58,
             ESP32H2_SPI_REG_BASE + 0x24,
             ESP32H2_SPI_REG_BASE + 0x28,
        },
         0x600B0800,
         { 0xd7b73e80, 0, 0, 0 },
         0x44,
         16,
        spi_config_unsupported,
        true,
    },

    // ESP32C6
    {
         {
             ESP32C6_SPI_REG_BASE + 0x00,
             ESP32C6_SPI_REG_BASE + 0x18,
             ESP32C6_SPI_REG_BASE + 0x1c,
             ESP32C6_SPI_REG_BASE + 0x20,
             ESP32C6_SPI_REG_BASE + 0x58,
             ESP32C6_SPI_REG_BASE + 0x24,
             ESP32C6_SPI_REG_BASE + 0x28,
        },
         0x600B0800,
         { 0x2CE0806F, 0, 0, 0 },
         0x44,
         13,
        spi_config_unsupported,
        true,
    },
};
typedef struct {
    const uint8_t *data;
    uint32_t size;
    uint32_t addr;
    const uint8_t *md5;
} partition_attr_t;

typedef struct {
    partition_attr_t boot;
    partition_attr_t part;
    partition_attr_t app;
} esp32_binaries_t;
typedef enum
{
    BOOT_COMBIN,
    PART,
    APP
} location_code_t;

typedef enum {
    SPI_FLASH_READ_ID = 0x9F
} spi_flash_cmd_t;
typedef struct __attribute__((packed))
{
    command_common_t common;
    uint32_t address;
    uint32_t value;
    uint32_t mask;
    uint32_t delay_us;
} write_reg_command_t;
#endif
