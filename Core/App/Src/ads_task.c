/*
 * ads_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "ads_task.h"
#include "ads1299.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


#define ADS_UART_HEADER_1  0xAA
#define ADS_UART_HEADER_2  0x55

void ADS_SendDataPointUART(UART_HandleTypeDef *huart,
                           const ads_data_point_t *data_point,
                           uint32_t pending,
                           uint8_t num_channels);

osThreadId_t adsTaskHandle;
static TaskHandle_t adsTaskNotifyHandle = NULL;
const osThreadAttr_t adsTaskAttributes = {
    .name = "adsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};


void adsTask(void *argument)
{
	adsTaskNotifyHandle = xTaskGetCurrentTaskHandle();
    uint8_t id;

    ADS_CS_HIGH(&ads_dev);

    vTaskDelay(pdMS_TO_TICKS(100));

    ADS_SendCommand(&ads_dev, ADS_RESET);

    vTaskDelay(pdMS_TO_TICKS(10));

    ADS_SendCommand(&ads_dev, ADS_SDATAC);

    vTaskDelay(pdMS_TO_TICKS(10));

    id = ADS_ReadRegister(&ads_dev, ADS_REG_ID);

    printf("ADS1299 ID: 0x%02X\r\n", id);

    vTaskDelay(pdMS_TO_TICKS(10));

    ADS_WriteRegister(&ads_dev, ADS_REG_CONFIG1, 0x96);      // 250 SPS
    ADS_WriteRegister(&ads_dev, ADS_REG_CONFIG2, 0xC0);		 // default
    ADS_WriteRegister(&ads_dev, ADS_REG_CONFIG3, 0xEC);		 // enable internal ref buffer, bias is connected, biasREF is internal

    ADS_WriteRegister(&ads_dev, ADS_REG_MISC1, 0x20);        // SRB1 common reference

    ADS_WriteRegister(&ads_dev, ADS_REG_CH1SET, 0x60);       // CH1 on, gain 24, normal input

    ADS_WriteRegister(&ads_dev, ADS_REG_CH2SET, 0x81); // default for turned off and input shorted
    ADS_WriteRegister(&ads_dev, ADS_REG_CH3SET, 0x81);
    ADS_WriteRegister(&ads_dev, ADS_REG_CH4SET, 0x81);
    ADS_WriteRegister(&ads_dev, ADS_REG_CH5SET, 0x81);
    ADS_WriteRegister(&ads_dev, ADS_REG_CH6SET, 0x81);
    ADS_WriteRegister(&ads_dev, ADS_REG_CH7SET, 0x81);
    ADS_WriteRegister(&ads_dev, ADS_REG_CH8SET, 0x81);

    ADS_WriteRegister(&ads_dev, ADS_REG_BIAS_SENSP, 0x01);   // CH1P contributes to bias
    ADS_WriteRegister(&ads_dev, ADS_REG_BIAS_SENSN, 0x00);

	ADS_SendCommand(&ads_dev, ADS_START);
	vTaskDelay(pdMS_TO_TICKS(10));


	ads_data_point_t data_point;

	HAL_TIM_Base_Start_IT(&htim6);
	uint32_t pending;
	while(1)
	{
		pending = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
	    ADS_ReadDataOnce(&ads_dev, &data_point);

	  /*  printf("TS=%lu %02X %02X %02X\r\n",
	           (unsigned long)data_point.timestamp,
	           data_point.status[0],
	           data_point.status[1],
	           data_point.status[2]);

	    for (uint8_t i = 0; i < ADS_NUM_CHANNELS; i++)
	    {
	        printf("CH%u=%ld ", i + 1, (long)data_point.channel[i]);
	    }

	    printf("\r\n");*/

	  // printf("%ld\r\n", (long)data_point.channel[0]);

	    // Custom UART printing of data_point
	    ADS_SendDataPointUART(&huart2, &data_point, pending, ADS_NUM_CHANNELS);

	   // vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void ADS_SendDataPointUART(UART_HandleTypeDef *huart,
                           const ads_data_point_t *data_point,
                           uint32_t pending,
                           uint8_t num_channels)
{
    if (num_channels > ADS_NUM_CHANNELS)
    {
        num_channels = ADS_NUM_CHANNELS;
    }

    uint8_t packet[2 + 1 + 1 + ADS_NUM_CHANNELS * 4];
    uint16_t idx = 0;

    packet[idx++] = ADS_UART_HEADER_1;
    packet[idx++] = ADS_UART_HEADER_2;

    packet[idx++] = (uint8_t)(data_point->timestamp & 0xFF);
    packet[idx++] = (uint8_t)(pending & 0xFF);

    for (uint8_t i = 0; i < num_channels; i++)
    {
        memcpy(&packet[idx], &data_point->channel[i], 4);
        idx += 4;
    }

    HAL_UART_Transmit(huart, packet, idx, HAL_MAX_DELAY);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        vTaskNotifyGiveFromISR(adsTaskNotifyHandle,
                               &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
