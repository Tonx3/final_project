/*
 * ads_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "ads_task.h"

#include <stdio.h>

#include "main.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern SPI_HandleTypeDef hspi1;

osThreadId_t adsTaskHandle;
const osThreadAttr_t adsTaskAttributes = {
    .name = "adsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

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

#define ADS_ID 0x00

//-------------------------------------------------------------------------------
// GPIO CONTROL
//-------------------------------------------------------------------------------
#define ADS_CS_LOW() HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_RESET)
#define ADS_CS_HIGH() HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_SET)

//-------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------
static void ADS_SendCommand(uint8_t cmd)
{
    ADS_CS_LOW();

    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);

    ADS_CS_HIGH();

    vTaskDelay(pdMS_TO_TICKS(1));
}

static uint8_t ADS_ReadRegister(uint8_t reg)
{
    uint8_t tx[3];
    uint8_t rx[3];

    tx[0] = ADS_RREAD | (reg & 0x1F); // RREG
    tx[1] = 0x00;       // read 1 register
    tx[2] = 0x00;

    ADS_CS_LOW();

    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, HAL_MAX_DELAY);

    ADS_CS_HIGH();

    return rx[2];
}

void adsTask(void *argument)
{

	while(1){

		  	  // 1. Pull CS Low
		  	  HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_RESET);

		  	  buffer[0] = 0x11;

		  	  // 3. Transmit the Address and Receive the Data simultaneously
		  	  if (HAL_SPI_TransmitReceive(&hspi1, buffer, read_data, 1, 100) == HAL_OK)
		  	  {
		  		  printf("Turned off read data continious hopefully\n\r");
		  	  }

		  	  // 4. Pull CS High
		  	  HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_SET);
		  	osDelay(1000);

		  	  uint8_t size = 0x18 + 2;     // Number of bytes to read

		  	  // 1. Pull CS Low
		  	  HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_RESET);

		  	  // 2. Transmit the Register Address (with read bit)
		  	  // Example: some sensors require an 0x80 read-bit flag added to the address
		  	  buffer[0] = 0x20 | ADS_ID;
		  	  buffer[1] = 0x17;

		  	  // 3. Transmit the Address and Receive the Data simultaneously
		  	  if (HAL_SPI_TransmitReceive(&hspi1, buffer, read_data, size, 1000) == HAL_OK)
		  	  {
		  		  printf("Sucess\n\r");
		  		  printf("(\n\r");
		  		  for(int i = 0; i < size-2; i++){
		  			  printf("%02X: %02X  \n\r", i, read_data[i+2]);
		  		  }
		  		  printf(")\n\r");
		  	      // Data is now in read_data array
		  	  }

		  	  // 4. Pull CS High
		  	  HAL_GPIO_WritePin(SPI1_CS_ADS_GPIO_Port, SPI1_CS_ADS_Pin, GPIO_PIN_SET);
		  	osDelay(1000);

	}

    uint8_t id;

    printf("ADS task start\r\n");

    ADS_CS_HIGH();

    vTaskDelay(pdMS_TO_TICKS(100));

    ADS_SendCommand(ADS_RESET);

    vTaskDelay(pdMS_TO_TICKS(10));

    ADS_SendCommand(ADS_SDATAC);

    vTaskDelay(pdMS_TO_TICKS(10));

    id = ADS_ReadRegister(ADS_ID);

    printf("ADS1299 ID: 0x%02X\r\n", id);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
