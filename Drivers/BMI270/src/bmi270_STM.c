/*
 * bmi270_platform.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "bmi270.h"

#include "i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define BMI270_I2C_ADDR UINT8_C(0x68)
#define READ_WRITE_LEN UINT8_C(16)

static struct bmi2_dev bmi_dev;

/*!
 * I2C read function map to STM platform
 */
BMI2_INTF_RETURN_TYPE bmi2_STM_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
	// intf_ptr will contain your I2C handle (e.g., I2C_HandleTypeDef)
	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)intf_ptr;

	// Use STM32 HAL to read from I2C
	if (HAL_I2C_Mem_Read(hi2c, BMI270_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT,
						 reg_data, len, HAL_MAX_DELAY) != HAL_OK)
	{
		return BMI2_E_COM_FAIL;
	}
	return BMI2_OK;
}

/*!
 * I2C write function map to STM platform
 */
BMI2_INTF_RETURN_TYPE bmi2_STM_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
	// intf_ptr will contain your I2C handle (e.g., I2C_HandleTypeDef)
	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)intf_ptr;

	// Use STM32 HAL to write to I2C
	if (HAL_I2C_Mem_Write(hi2c, BMI270_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT,
						  (uint8_t *)reg_data, len, HAL_MAX_DELAY) != HAL_OK)
	{
		return BMI2_E_COM_FAIL;
	}
	return BMI2_OK;
}

/*!
 * Delay function map to STM platform
 */
void bmi2_STM_delay_us(uint32_t period, void *intf_ptr)
{
	vTaskDelay(pdMS_TO_TICKS((period + 999 )/1000));
}

void bmi270_STM_init()
{
	bmi_dev.intf = BMI2_I2C_INTF;
	bmi_dev.intf_ptr = (void *)&hi2c1; // change to specific I2C handle pcb
	bmi_dev.read = bmi2_STM_i2c_read;
	bmi_dev.write = bmi2_STM_i2c_write;
	bmi_dev.delay_us = bmi2_STM_delay_us;
	bmi_dev.read_write_len = READ_WRITE_LEN;
	bmi_dev.config_file_ptr = NULL;

	int8_t rslt = bmi270_init(&bmi_dev);
	if (rslt != BMI2_OK)
	{
		// Handle error
		printf("BMI2 init failed: %d\n\r", rslt);
		return;
	}

	// Device is now ready
	printf("BMI2 initialized. Chip ID: 0x%02X\n\r", bmi_dev.chip_id);
}
