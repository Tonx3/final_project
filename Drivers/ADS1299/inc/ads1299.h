/*
 * ads1299.h
 *
 *  Created on: 14. lip 2026.
 *      Author: ANTONIA
 */

#ifndef ADS1299_INC_ADS1299_H_
#define ADS1299_INC_ADS1299_H_

//-------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------
#include <stdio.h>

#include "main.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


//-------------------------------------------------------------------------------
// ADS1299 COMMANDS
//-------------------------------------------------------------------------------
#define ADS_WAKEUP 0x02
#define ADS_STANDBY 0x04
#define ADS_RESET 0x06
#define ADS_START 0x08
#define ADS_STOP 0x0A
#define ADS_RDATAC 0x10
#define ADS_SDATAC 0x11
#define ADS_RDATA 0x12
#define ADS_RREG 0x20
#define ADS_WREG 0x40


//-------------------------------------------------------------------------------
// ADS1299 REGISTERS
//-------------------------------------------------------------------------------
/* Read-only ID register */
#define ADS_REG_ID             0x00

/* Global configuration registers */
#define ADS_REG_CONFIG1        0x01
#define ADS_REG_CONFIG2        0x02
#define ADS_REG_CONFIG3        0x03
#define ADS_REG_LOFF           0x04

/* Channel configuration registers */
#define ADS_REG_CH1SET         0x05
#define ADS_REG_CH2SET         0x06
#define ADS_REG_CH3SET         0x07
#define ADS_REG_CH4SET         0x08
#define ADS_REG_CH5SET         0x09
#define ADS_REG_CH6SET         0x0A
#define ADS_REG_CH7SET         0x0B
#define ADS_REG_CH8SET         0x0C

/* Bias drive configuration */
#define ADS_REG_BIAS_SENSP     0x0D
#define ADS_REG_BIAS_SENSN     0x0E

/* Lead-off detection configuration */
#define ADS_REG_LOFF_SENSP     0x0F
#define ADS_REG_LOFF_SENSN     0x10
#define ADS_REG_LOFF_FLIP      0x11

/* Lead-off status registers (read-only) */
#define ADS_REG_LOFF_STATP     0x12
#define ADS_REG_LOFF_STATN     0x13

/* General-purpose I/O */
#define ADS_REG_GPIO           0x14

/* Miscellaneous configuration */
#define ADS_REG_MISC1          0x15
#define ADS_REG_MISC2          0x16
#define ADS_REG_CONFIG4        0x17

//-------------------------------------------------------------------------------
// DEFINES
//-------------------------------------------------------------------------------
#define ADS_CS_LOW(dev)  HAL_GPIO_WritePin((dev)->cs_port, (dev)->cs_pin, GPIO_PIN_RESET)
#define ADS_CS_HIGH(dev) HAL_GPIO_WritePin((dev)->cs_port, (dev)->cs_pin, GPIO_PIN_SET)

#define ADS_NUM_CHANNELS 8
#define ADS_STATUS_BYTES   3
#define ADS_BYTES_PER_CH   3
#define ADS_FRAME_BYTES    (ADS_STATUS_BYTES + ADS_NUM_CHANNELS * ADS_BYTES_PER_CH)

//-------------------------------------------------------------------------------
// STRUCT
//-------------------------------------------------------------------------------
typedef struct
{
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
} ads1299_dev_t;

typedef struct
{
    TickType_t timestamp;
    uint8_t status[3];
    int32_t channel[ADS_NUM_CHANNELS];   // sign-extended 24-bit values
} ads_data_point_t;

extern ads1299_dev_t ads_dev;

//-------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------
void ADS_SendCommand(ads1299_dev_t *dev, uint8_t cmd);
uint8_t ADS_ReadRegister(ads1299_dev_t *dev, uint8_t reg);
void ADS_WriteRegister(ads1299_dev_t *dev, uint8_t reg, uint8_t value);
void ADS_ReadFirstNChannels(ads1299_dev_t *dev, uint8_t n, ads_data_point_t* data_point);
void ADS_ReadAllChannels(ads1299_dev_t *dev, ads_data_point_t* data_point);
void ADS_ReadDataOnce(ads1299_dev_t *dev, ads_data_point_t *data_point);



#endif /* ADS1299_INC_ADS1299_H_ */
