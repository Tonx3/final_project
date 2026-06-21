/*
 * ads_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "ads_task.h"
#include "ads1299.h"

#include <stdio.h>

#include "main.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId_t adsTaskHandle;
const osThreadAttr_t adsTaskAttributes = {
    .name = "adsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};


void adsTask(void *argument)
{
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

	for (;;)
	{
	    ADS_ReadDataOnce(&ads_dev, &data_point);

	 /*   printf("TS=%lu %02X %02X %02X\r\n",
	           (unsigned long)data_point.timestamp,
	           data_point.status[0],
	           data_point.status[1],
	           data_point.status[2]);

	    for (uint8_t i = 0; i < ADS_NUM_CHANNELS; i++)
	    {
	        printf("CH%u=%ld ", i + 1, (long)data_point.channel[i]);
	    }

	    printf("\r\n");
*/

	   printf("%ld\r\n", (long)data_point.channel[0]);

	   // vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
