#include <stddef.h>
#include "sd_card.h"

#define __SD_CARD_CMD0 0
#define __SD_CARD_GO_IDLE __SD_CARD_CMD0

#define __SD_CARD_CMD1 1
#define __SD_CARD_ALL_SEND_OP_COND __SD_CARD_CMD1

#define __SD_CARD_CMD6 6
#define __SD_CARD_SWITCH_FUNC __SD_CARD_CMD6

#define __SD_CARD_CMD8 8
#define __SD_CARD_SEND_IF_COND __SD_CARD_CMD8

#define __SD_CARD_CMD9 9
#define __SD_CARD_SEND_CSD __SD_CARD_CMD9

#define __SD_CARD_CMD10 10
#define __SD_CARD_SEND_CID __SD_CARD_CMD10

#define __SD_CARD_CMD12 12
#define __SD_CARD_STOP_TRANSMISSION __SD_CARD_CMD12

#define __SD_CARD_CMD13 13
#define __SD_CARD_SEND_STATUS __SD_CARD_CMD13

#define __SD_CARD_CMD16 16
#define __SD_CARD_SET_BLOCK_LEN __SD_CARD_CMD16

#define __SD_CARD_CMD17 17
#define __SD_CARD_READ_SINGLE_BLOCK __SD_CARD_CMD17

#define __SD_CARD_CMD18 18
#define __SD_CARD_READ_MULTIPLE_BLOCK __SD_CARD_CMD18

#define __SD_CARD_CMD24 24
#define __SD_CARD_WRITE_BLOCK __SD_CARD_CMD24

#define __SD_CARD_CMD25 25
#define __SD_CARD_WRITE_MULTIPLE_BLOCK __SD_CARD_CMD25

#define __SD_CARD_CMD27 27
#define __SD_CARD_PROGRAM_CSD __SD_CARD_CMD27

#define __SD_CARD_CMD28 28
#define __SD_CARD_SET_WRITE_PROT __SD_CARD_CMD28

#define __SD_CARD_CMD29 29
#define __SD_CARD_CLR_WRITE_PROT __SD_CARD_CMD29

#define __SD_CARD_CMD30 30
#define __SD_CARD_SEND_WRITE_PROT __SD_CARD_CMD30

#define __SD_CARD_CMD32 32
#define __SD_CARD_ERASE_WR_BLK_START __SD_CARD_CMD32

#define __SD_CARD_CMD33 33
#define __SD_CARD_ERASE_WR_BLK_END __SD_CARD_CMD33

#define __SD_CARD_CMD38 38
#define __SD_CARD_ERASE __SD_CARD_CMD38

#define __SD_CARD_CMD42 42
#define __SD_CARD_LOCK_UNLOCK __SD_CARD_CMD42

#define __SD_CARD_CMD55 55
#define __SD_CARD_APP_CMD __SD_CARD_CMD55

#define __SD_CARD_CMD56 56
#define __SD_CARD_GEN_CMD __SD_CARD_CMD56

#define __SD_CARD_CMD58 58
#define __SD_CARD_READ_OCR __SD_CARD_CMD58

#define __SD_CARD_CMD59 59
#define __SD_CARD_CRC_ON_OFF __SD_CARD_CMD59

#define __SD_CARD_ACMD13 13
#define __SD_CARD_SD_STATUS __SD_CARD_ACMD13

#define __SD_CARD_ACMD22 22
#define __SD_CARD_SEND_NUM_WR_BLOCKS __SD_CARD_ACMD22

#define __SD_CARD_ACMD23 23
#define __SD_CARD_SET_WR_BLK_ERASE_COUNT __SD_CARD_ACMD23

#define __SD_CARD_ACMD41 41
#define __SD_CARD_SD_SEND_OP_COND __SD_CARD_ACMD41

#define __SD_CARD_ACMD42 42
#define __SD_CARD_SET_CLR_CARD_DETECT __SD_CARD_ACMD42

#define __SD_CARD_ACMD51 51
#define __SD_CARD_SEND_SCR __SD_CARD_ACMD51

#define __SD_CARD_START_BLOCK_TOKEN 0xFEU
#define __SD_CARD_START_BLOCK_TOKEN_CMD25 0xFCU
#define __SD_CARD_STOP_TRAN_TOKEN 0xFDU


#define __SD_CARD_VOLTAGE_2_7V_3_6V (0x1UL << 8)
#define __SD_CARD_CHECK_PATTERN 0xAAUL

#define __SD_CARD_HCS (0x1UL << 30)
#define __SD_CARD_CCS (0x1UL << 30)

#define __SD_CARD_DUMMY_BYTE 0xFF
#define __SD_CARD_MAX_RETRIES 5000
#define __SD_CARD_DEFAULT_BLOCK_LEN 512
#define __SD_CARD_CMD0_CRC 0x94U
#define __SD_CARD_CMD8_CRC 0x86U

#define __SD_CARD_ASSERT(param) do \
{ \
	uint32_t val = (param); \
	if (!val) \
	{ \
		return SD_CARD_ERROR; \
	} \
}while(0)

#define __SD_CARD_RETURN_ERROR(param) do \
{ \
	sd_card_return_type val = (param); \
	if (val != SD_CARD_OK) \
	{ \
		return val; \
	} \
}while(0)



struct __attribute__((packed)) __sd_card_command_type
{
	const uint8_t start:1;
	uint8_t direction:1;
	uint8_t command_index:6;
	uint32_t argument;
	uint8_t crc:7;
	const uint8_t end:1;
};


static sd_card_driver_type const* __drv;

static inline void __sd_card_clear_card_errors(sd_card_type * sd_card);
static sd_card_return_type __sd_card_cmd_r1_helper(sd_card_type * sd_card, struct __sd_card_command_type cmd);
static sd_card_return_type __sd_card_cmd_r1(sd_card_type * sd_card, uint8_t cmd_idx, uint32_t arg);
static sd_card_return_type __sd_card_cmd_r1b(sd_card_type * sd_card, uint8_t cmd_idx, uint32_t arg);
static sd_card_return_type __sd_card_cmd_r2(sd_card_type * sd_card, uint8_t cmd_idx, uint32_t arg);
static sd_card_return_type __sd_card_cmd_r3_7(sd_card_type * sd_card, uint8_t cmd_idx, uint32_t arg, uint32_t *rsp);

static sd_card_return_type __sd_card_cmd0_init(sd_card_type * sd_card);
static sd_card_return_type __sd_card_cmd8_init(sd_card_type * sd_card);

static sd_card_return_type __sd_card_read(sd_card_type *card, uint8_t cmd_idx, uint32_t arg, uint8_t *block, uint32_t block_len, uint32_t block_num, uint16_t *crc16);
static sd_card_return_type __sd_card_write(sd_card_type *card, uint8_t cmd_idx, uint32_t arg, uint8_t *block, uint32_t block_len, uint32_t block_num, uint16_t *crc16);

static inline void __sd_card_clear_card_errors(sd_card_type *card)
{
    card->error.command = 0;
    card->error.r1 = 0;
    card->error.r2 = 0;
    card->error.is_command_timeout = false;
    card->error.is_command_busy = false;
    card->error.read_token = 0;
    card->error.is_read_timeout = false;
    card->error.write_response_token = 0;
	card->error.is_write_timeout = false;
    card->error.is_command_busy = false;	
}

static sd_card_return_type __sd_card_cmd_r1_helper(sd_card_type *sd_card, struct __sd_card_command_type cmd)
{
	uint8_t r1 = __SD_CARD_DUMMY_BYTE;
	uint32_t retries = 0;

	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

	__drv->spi_write((uint8_t*)&cmd, sizeof(cmd));
	
	/*Attempt to receive a response until the sd card responds with anything other than 0xFF or max retries are reached.*/
	while(r1 == __SD_CARD_DUMMY_BYTE && retries++ < __SD_CARD_MAX_RETRIES)
	{
		__drv->spi_read(&r1, 1);
	}
	
	if(retries >= __SD_CARD_MAX_RETRIES)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.is_command_timeout = true;
		return SD_CARD_FAULT;
	}
	else if(r1)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.r1 = r1;
		return SD_CARD_FAULT;
	}

	return SD_CARD_OK;
}

static sd_card_return_type __sd_card_cmd_r1(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg)
{
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);


#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif
	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);
	sd_card->spi_set_cs(1);
	return ret;
}

static sd_card_return_type __sd_card_cmd_r1b(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg)
{
	uint8_t busy = 0;
	uint32_t retries = 0;
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};

	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);

	if(ret != SD_CARD_OK)
	{
		goto end_label;
	}
	
	/*Attempt to receive a response, until the sd card responds with anything other than 0 or max retries are reached.*/
	while(!busy && retries++ < __SD_CARD_MAX_RETRIES)
	{
		__drv->spi_read(&busy, 1);
	}

	if(retries == __SD_CARD_MAX_RETRIES)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.is_command_busy = true;		
		ret = SD_CARD_FAULT;
	}

end_label:	
	sd_card->spi_set_cs(1);	
	return ret;
}

static sd_card_return_type __sd_card_cmd_r2(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg)
{
	uint8_t r2 = 0;
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif
	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);	
	__drv->spi_read(&r2, 1);
	sd_card->spi_set_cs(1);
	
	if (r2)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.r2 = r2;
		ret = SD_CARD_FAULT;
	}

	return ret; 
}

static sd_card_return_type __sd_card_cmd_r3_7(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg, uint32_t *rsp)
{
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);
	if(ret == SD_CARD_OK)
	{
		__drv->spi_read((uint8_t*)&rsp, 4);
	}
	sd_card->spi_set_cs(1);
	return ret;
}

static sd_card_return_type __sd_card_cmd0_init(sd_card_type *sd_card)
{
	uint8_t r1 = __SD_CARD_DUMMY_BYTE;
	uint32_t retries = 0;
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (__SD_CARD_CMD0 & 0x3FU),
		.crc = (__SD_CARD_CMD0_CRC >> 1),
		.end = 1
	};
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	__drv->spi_write((uint8_t*)&cmd, sizeof(cmd));
	
	while(r1 == __SD_CARD_DUMMY_BYTE && retries++ < __SD_CARD_MAX_RETRIES)
	{
		__drv->spi_read(&r1, 1);
	}
	sd_card->spi_set_cs(1);
	
	if(retries == __SD_CARD_MAX_RETRIES)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.is_command_timeout = true;
		ret = SD_CARD_FAULT;
	}
	else if(r1 & SD_CARD_IN_IDLE_STATE)
	{
		ret = SD_CARD_OK;
	}
	else
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.r1 = r1;
		ret = SD_CARD_FAULT;
	}

	return ret;
}

static sd_card_return_type __sd_card_cmd8_init(sd_card_type *sd_card)
{
	sd_card_return_type ret = SD_CARD_OK;
	uint8_t r1 = __SD_CARD_DUMMY_BYTE;
	uint32_t resp = 0, retries = 0;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (__SD_CARD_CMD8 & 0x3FU),
		.argument = (__SD_CARD_VOLTAGE_2_7V_3_6V | __SD_CARD_CHECK_PATTERN),
		.crc = (__SD_CARD_CMD8_CRC >> 1),
		.end = 1
	};

	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	__drv->spi_write((uint8_t*)&cmd, sizeof(cmd));
	
	while(r1 == __SD_CARD_DUMMY_BYTE && retries++ < __SD_CARD_MAX_RETRIES)
	{
		__drv->spi_read(&r1, 1);
	}

	if(retries == __SD_CARD_MAX_RETRIES || r1 & SD_CARD_ILLEGAL_COMMAND)
	{
		sd_card->is_v2 = false;
		goto end_label;
	}
	else if(r1 & SD_CARD_IN_IDLE_STATE)
	{
		__drv->spi_read((uint8_t*)&resp, 4);
		if (resp & (__SD_CARD_VOLTAGE_2_7V_3_6V | __SD_CARD_CHECK_PATTERN))
		{
			sd_card->is_v2 = true;			
			goto end_label;
		}
	}
	sd_card->error.command = cmd.command_index;
	sd_card->error.r1 = r1;
	ret = SD_CARD_FAULT;
end_label:
	sd_card->spi_set_cs(1);
	return ret;
}

static sd_card_return_type __sd_card_read(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg, uint8_t *block, uint32_t block_len, uint32_t block_num, uint16_t *crc16)
{
	uint8_t s_bl_tkn = 0, *dst = block, *crc = (uint8_t*)crc16;
	uint32_t cnt = block_num, retries = 0;
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);

	if(ret != SD_CARD_OK)
	{
		goto end_label;
	}

	while(cnt--)
	{
		s_bl_tkn = __SD_CARD_DUMMY_BYTE;
		retries = 0;
		/*Try to read the start block token.*/
		while(s_bl_tkn == __SD_CARD_DUMMY_BYTE && retries++ < __SD_CARD_MAX_RETRIES)
		{
			__drv->spi_read(&s_bl_tkn, 1);
		}

		if (retries >= __SD_CARD_MAX_RETRIES)
		{
			sd_card->error.command = cmd.command_index;
			sd_card->error.is_read_timeout = true;
			ret = SD_CARD_FAULT;
			break;
		}

		if(s_bl_tkn != __SD_CARD_START_BLOCK_TOKEN)
		{
			sd_card->error.command = cmd.command_index;
			sd_card->error.read_token = s_bl_tkn;
			ret = SD_CARD_FAULT;
			break;
		}

		__drv->spi_read(dst, block_len);
		__drv->spi_read(crc, 2);

		dst += block_len;
		crc += 2;
	}

	if(block_num == 1)
	{
		goto end_label;
	}

	cmd.command_index = __SD_CARD_STOP_TRANSMISSION;

#ifdef SD_CARD_CRC	
	if(sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif
	if(ret != SD_CARD_OK)
	{
		(void)__sd_card_cmd_r1_helper(sd_card, cmd);
	}
	else
	{
		ret = __sd_card_cmd_r1_helper(sd_card, cmd);
	}
	
	
end_label:
	sd_card->spi_set_cs(1);
	return ret;
}

static sd_card_return_type __sd_card_write(sd_card_type *sd_card, uint8_t cmd_idx, uint32_t arg, uint8_t *block, uint32_t block_len, uint32_t block_num, uint16_t *crc16)
{
	uint8_t busy = 0, r_tkn = 0, s_bl_tkn = ((cmd_idx == __SD_CARD_CMD25) ? __SD_CARD_START_BLOCK_TOKEN_CMD25 : __SD_CARD_START_BLOCK_TOKEN), *src = block, *crc = (uint8_t*)crc16;
	uint32_t cnt = block_num, retries = 0;
	sd_card_return_type ret = SD_CARD_OK;
	struct __sd_card_command_type cmd = 
	{
		.start = 0,
		.direction = 1,
		.command_index = (cmd_idx & 0x3FU),
		.argument = arg,
		.end = 1
	};
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on == true)
	{
		cmd.crc = __drv->crc7calc((uint8_t*)&cmd, (sizeof(cmd) - 1));
	}
#endif

	__sd_card_clear_card_errors(sd_card);
	sd_card->spi_set_cs(0);
	ret = __sd_card_cmd_r1_helper(sd_card, cmd);

	if(ret != SD_CARD_OK)
	{
		goto end_label;
	}

	while(cnt--)
	{
		__drv->spi_write(&s_bl_tkn, 1);
		__drv->spi_write(src, block_len);
		__drv->spi_write(crc, 2);

		retries = 0;
		r_tkn = __SD_CARD_DUMMY_BYTE;

		while(r_tkn == __SD_CARD_DUMMY_BYTE && retries++ < __SD_CARD_MAX_RETRIES)
		{
			__drv->spi_read(&r_tkn, 1);
		}

		if (retries >= __SD_CARD_MAX_RETRIES)
		{
			sd_card->error.command = cmd.command_index;
			sd_card->error.is_command_timeout = true;
			ret = SD_CARD_FAULT;
			break;
		}

		if(r_tkn != SD_CARD_DATA_RESPONSE_ACCEPTED_TOKEN)
		{
			sd_card->error.command = cmd.command_index;
			sd_card->error.write_response_token = r_tkn;
			ret = SD_CARD_FAULT;
			break;
		}

		retries = 0;
		busy = 0;
		while(!busy && retries++ < __SD_CARD_MAX_RETRIES)
		{
			__drv->spi_read(&busy, 1);
		}

		if (retries >= __SD_CARD_MAX_RETRIES)
		{
			sd_card->error.command = cmd.command_index;
			sd_card->error.is_write_busy = true;
			ret = SD_CARD_FAULT;
			break;
		}

		src += block_len;
		crc += 2;
	}

	if(block_num == 1)
	{
		goto end_label;
	}

	s_bl_tkn = __SD_CARD_STOP_TRAN_TOKEN;
	__drv->spi_write(&s_bl_tkn, 1);
	retries = 0;
	busy = 0;
	while(!busy && retries++ < __SD_CARD_MAX_RETRIES)
	{
		__drv->spi_read(&busy, 1);
	}

	if (retries >= __SD_CARD_MAX_RETRIES)
	{
		sd_card->error.command = cmd.command_index;
		sd_card->error.is_write_busy = true;
		ret = SD_CARD_FAULT;
	}

end_label:
	sd_card->spi_set_cs(1);
	return ret;
}

sd_card_return_type sd_card_spi_setup(sd_card_driver_type const * const driver)
{
	__SD_CARD_ASSERT(driver != NULL);
	__SD_CARD_ASSERT(driver->spi_io_setup != NULL);
	__SD_CARD_ASSERT(driver->spi_read != NULL);
	__SD_CARD_ASSERT(driver->spi_write != NULL);
#ifdef SD_CARD_CRC
	__SD_CARD_ASSERT(driver->crc7calc != NULL);
	__SD_CARD_ASSERT(driver->crc16calc != NULL);
#endif

	__drv = driver;
	driver->spi_io_setup();
	return SD_CARD_OK;
}

sd_card_return_type sd_card_init(sd_card_type *sd_card)
{
	uint8_t dummy[74] = { 0 };
	uint16_t crc16 = 0;
	uint32_t retries = 0;

	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->spi_cs_io_setup != NULL);
	__SD_CARD_ASSERT(sd_card->spi_set_cs != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == false);

	sd_card->spi_cs_io_setup();

	for (uint32_t i = 0; i < sizeof(dummy); i++)
	{
		dummy[i] = 0xFF;
	}

	sd_card->spi_set_cs(1);
	__drv->spi_write(dummy, sizeof(dummy));

	/*Send CMD0*/
	__SD_CARD_RETURN_ERROR(__sd_card_cmd0_init(sd_card));


	/*Send CMD8*/
	__SD_CARD_RETURN_ERROR(__sd_card_cmd8_init(sd_card));

	/*Retry ACMD41 until it succeeds or .*/
	do
	{
		(void)__sd_card_cmd_r1(sd_card, __SD_CARD_APP_CMD, 0);
		(void)__sd_card_cmd_r1(sd_card, __SD_CARD_SD_SEND_OP_COND, ((sd_card->is_v2 == true) ? __SD_CARD_HCS : 0));
	}while((sd_card->error.r1 & SD_CARD_IN_IDLE_STATE) && retries++ < __SD_CARD_MAX_RETRIES);

	if (!retries)
	{
		return SD_CARD_FAULT;
	}

	if(sd_card->is_v2)
	{
		__SD_CARD_RETURN_ERROR(__sd_card_cmd_r3_7(sd_card, __SD_CARD_READ_OCR, 0, &sd_card->ocr));
	}

	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1(sd_card, __SD_CARD_SET_BLOCK_LEN, __SD_CARD_DEFAULT_BLOCK_LEN));
	__SD_CARD_RETURN_ERROR(__sd_card_read(sd_card, __SD_CARD_SEND_CSD, 0, (uint8_t*)&sd_card->csd, sizeof(sd_card->csd), 1, &crc16));
	__SD_CARD_RETURN_ERROR(__sd_card_read(sd_card, __SD_CARD_SEND_CID, 0, (uint8_t*)&sd_card->cid, sizeof(sd_card->cid), 1, &crc16));

	sd_card->is_initialized = true;
	return SD_CARD_OK;
}

sd_card_return_type sd_card_data_read(sd_card_type *sd_card, uint32_t address, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t block_num = (buffer_size % __SD_CARD_DEFAULT_BLOCK_LEN) ? (1 + (buffer_size / __SD_CARD_DEFAULT_BLOCK_LEN)) : (buffer_size / __SD_CARD_DEFAULT_BLOCK_LEN);
	uint16_t crc[block_num];
	uint8_t *dst = buffer, cmd = (block_num == 1) ? __SD_CARD_READ_SINGLE_BLOCK : __SD_CARD_READ_MULTIPLE_BLOCK;

	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == true);

	/*READ_BL_PARTIAL bit.*/
	if(!(sd_card->csd[7] & 0x80U) && buffer_size < __SD_CARD_DEFAULT_BLOCK_LEN)
	{
		return SD_CARD_FAULT;
	}

	__SD_CARD_RETURN_ERROR(__sd_card_read(sd_card, cmd, address, dst, buffer_size, block_num, crc));

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on)
	{
		for (uint32_t i = 0; i < block_num; i++)
		{
			if (crc[i] != __drv->crc16calc(dst, buffer_size))
			{
				return SD_CARD_READ_CRC_ERROR;
			}
			dst += buffer_size;
		}
	}
#endif

	return SD_CARD_OK;
}

sd_card_return_type sd_card_data_erase(sd_card_type *sd_card, uint32_t address, uint32_t data_size)
{
	uint32_t block_num = (data_size % __SD_CARD_DEFAULT_BLOCK_LEN) ? (1 + (data_size / __SD_CARD_DEFAULT_BLOCK_LEN)) : (data_size / __SD_CARD_DEFAULT_BLOCK_LEN);
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == true);

	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1(sd_card, __SD_CARD_ERASE_WR_BLK_START, address));
	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1(sd_card, __SD_CARD_ERASE_WR_BLK_END, (address + block_num)));
	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1b(sd_card, __SD_CARD_ERASE, 0));

	return SD_CARD_OK;
}


sd_card_return_type sd_card_data_write(sd_card_type *sd_card, uint32_t address, uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t block_num = (buffer_size % __SD_CARD_DEFAULT_BLOCK_LEN) ? (1 + (buffer_size / __SD_CARD_DEFAULT_BLOCK_LEN)) : (buffer_size / __SD_CARD_DEFAULT_BLOCK_LEN);
	uint16_t crc[block_num];
	uint8_t *src = buffer, cmd = (block_num == 1) ? __SD_CARD_WRITE_BLOCK : __SD_CARD_WRITE_MULTIPLE_BLOCK;
	
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == true);

#ifdef SD_CARD_CRC
	if (sd_card->is_crc_on)
	{
		for(uint32_t i = 0; i < block_num; i++)
		{
			crc[i] = __drv->crc16calc(src, buffer_size);
			src += buffer_size;
		}
	}
#endif

	__SD_CARD_RETURN_ERROR(__sd_card_write(sd_card, cmd, address, buffer, buffer_size, block_num, crc));

	return SD_CARD_OK;
}

#ifdef SD_CARD_CRC
sd_card_return_type sd_card_crc_on(sd_card_type *sd_card)
{
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == true);

	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1(sd_card, __SD_CARD_CRC_ON_OFF, 1));

	sd_card->is_crc_on == true;

	return SD_CARD_OK;
}

sd_card_return_type sd_card_off(sd_card_type *sd_card)
{
	__SD_CARD_ASSERT(__drv != NULL);
	__SD_CARD_ASSERT(sd_card != NULL);
	__SD_CARD_ASSERT(sd_card->is_initialized == true);

	__SD_CARD_RETURN_ERROR(__sd_card_cmd_r1(sd_card, __SD_CARD_CRC_ON_OFF, 0));
	sd_card->is_crc_on == false;
	return SD_CARD_OK;
}
#endif