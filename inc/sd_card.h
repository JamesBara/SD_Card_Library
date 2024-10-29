#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdint.h>
#include <stdbool.h>

#ifndef __GNUC__
#error Unsupported compiler!
#endif


#define SD_CARD_IN_IDLE_STATE (0x1U << 0)
#define SD_CARD_ERASE_RESET (0x1U << 1)
#define SD_CARD_ILLEGAL_COMMAND (0x1U << 2) 
#define SD_CARD_COMMAND_CRC_ERROR (0x1U << 3)
#define SD_CARD_ERASE_SEQUENCE_ERROR (0x1U << 4)
#define SD_CARD_ADDRESS_ERROR (0x1U << 5)
#define SD_CARD_PARAMETER_ERROR (0x1U << 6)
#define SD_CARD_IS_LOCKED (0x1U << 0)
#define SD_CARD_WP_ERASE_SKIP (0x1U << 1)
#define SD_CARD_LOCK_UNLOCK_CMD_FAILED SD_CARD_WP_ERASE_SKIP
#define SD_CARD_ERROR (0x1U << 2)
#define SD_CARD_CC_ERROR (0x1U << 3)
#define SD_CARD_ECC_FAILED (0x1U << 4)
#define SD_CARD_WP_VIOLATION (0x1U << 5)
#define SD_CARD_ERASE_PARAM (0x1U << 6)
#define SD_CARD_OUT_OF_RANGE (0x1U << 7)
#define SD_CARD_CSD_OVERWRITE SD_CARD_OUT_OF_RANGE

#define SD_CARD_DATA_RESPONSE_ACCEPTED_TOKEN 0x5U
#define SD_CARD_DATA_RESPONSE_CRC_ERROR_TOKEN 0xBU
#define SD_CARD_DATA_RESPONSE_WRITE_ERROR_TOKEN 0xDU

#define SD_CARD_ERROR_TOKEN 0x1U
#define SD_CARD_CC_ERROR_TOKEN 0x2U
#define SD_CARD_ECC_FAILED_ERROR_TOKEN 0x4U
#define SD_CARD_OUT_OF_RANGE_ERROR_TOKEN 0x8U
#define SD_CARD_CARD_IS_LOCKED_ERROR_TOKEN 0x10U

typedef enum __sd_card_return_type
{
    SD_CARD_OK = 0,
    SD_CARD_FAULT,
    SD_CARD_READ_CRC_ERROR
}sd_card_return_type;

typedef struct __sd_card_error_type
{
    uint8_t command;
    uint8_t r1;
    uint8_t r2;
    bool is_command_timeout;
    bool is_command_busy;
    uint8_t read_token;
    bool is_read_timeout;
    uint8_t write_response_token;
    bool is_write_timeout;
    bool is_write_busy;
}sd_card_error_type;

typedef struct __sd_card_type
{
    void (*spi_cs_io_setup)(void);
    void (*spi_set_cs)(uint8_t bit);
    bool is_initialized;
    bool is_v2;
#ifdef SD_CARD_CRC
    bool is_crc_on;
#endif
    uint32_t ocr;
    uint8_t cid[16];
    uint8_t csd[16];
    sd_card_error_type error;
}sd_card_type;

typedef struct __sd_card_driver_type
{
    void (*spi_io_setup)(void);
    void (*spi_write)(uint8_t *buffer, uint32_t buffer_size);
    void (*spi_read)(uint8_t *buffer, uint32_t buffer_size);
#ifdef SD_CARD_CRC
    uint8_t(*crc7calc)(uint8_t* data, uint32_t length);
    uint16_t(*crc16calc)(uint8_t* data, uint32_t length);
#endif
}sd_card_driver_type;



sd_card_return_type sd_card_spi_setup(sd_card_driver_type const * const driver);
sd_card_return_type sd_card_init(sd_card_type *sd_card);
sd_card_return_type sd_card_data_read(sd_card_type *card, uint32_t address, uint8_t *buffer, uint32_t buffer_size);
sd_card_return_type sd_card_data_erase(sd_card_type *card, uint32_t address, uint32_t block_number);
sd_card_return_type sd_card_data_write(sd_card_type *card, uint32_t address, uint8_t *buffer, uint32_t buffer_size);

#ifdef SD_CARD_CRC
    sd_card_return_type sd_card_crc_on(sd_card_type *card);
    sd_card_return_type sd_card_crc_off(sd_card_type *card);
#endif






























#endif /*SD_CARD_H*/