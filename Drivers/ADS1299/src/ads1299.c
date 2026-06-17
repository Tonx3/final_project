/*
 * ads1299.c
 *
 *  Created on: 14. lip 2026.
 *      Author: ANTONIA
 */

#include "ads1299.h"

ads1299_dev_t ads_dev =
{
    .spi_handle = &hspi1,

    .cs_port = SPI1_CS_ADS_GPIO_Port,
    .cs_pin  = SPI1_CS_ADS_Pin
};

//-------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------
static int32_t ADS_Parse24Bit(const uint8_t *buf);

//-------------------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------------------
void ADS_SendCommand(ads1299_dev_t *dev, uint8_t cmd)
{
    ADS_CS_LOW(dev);

    HAL_SPI_Transmit(dev->spi_handle, &cmd, 1, HAL_MAX_DELAY);

    ADS_CS_HIGH(dev);

    vTaskDelay(pdMS_TO_TICKS(1));
}

uint8_t ADS_ReadRegister(ads1299_dev_t *dev, uint8_t reg)
{
    uint8_t tx[3];
    uint8_t rx[3];

    tx[0] = ADS_RREG | (reg & 0x1F); // RREG
    tx[1] = 0x00;       // read 1 register
    tx[2] = 0x00;

    ADS_CS_LOW(dev);

    HAL_SPI_TransmitReceive(dev->spi_handle, tx, rx, 3, HAL_MAX_DELAY);

    ADS_CS_HIGH(dev);

    return rx[2];
}

void ADS_WriteRegister(ads1299_dev_t *dev, uint8_t reg, uint8_t value)
{
    uint8_t tx[3];

    tx[0] = ADS_WREG | (reg & 0x1F);
    tx[1] = 0x00;       // write 1 register
    tx[2] = value;

    ADS_CS_LOW(dev);

    HAL_SPI_Transmit(dev->spi_handle, tx, 3, HAL_MAX_DELAY);

    ADS_CS_HIGH(dev);
}

void ADS_ReadFirstNChannels(ads1299_dev_t *dev, uint8_t n, ads_data_point_t* data_point)
{
	if (n > ADS_NUM_CHANNELS)
	{
		n = ADS_NUM_CHANNELS;
	}

	uint8_t tx[ADS_FRAME_BYTES] = {0};
	uint8_t rx[ADS_FRAME_BYTES] = {0};

	uint16_t len = ADS_STATUS_BYTES + n * ADS_BYTES_PER_CH;

	ADS_CS_LOW(dev);

	HAL_SPI_TransmitReceive(dev->spi_handle,
							tx,
							rx,
							len,
							HAL_MAX_DELAY);

	ADS_CS_HIGH(dev);

	data_point->timestamp = xTaskGetTickCount();

	data_point->status[0] = rx[0];
	data_point->status[1] = rx[1];
	data_point->status[2] = rx[2];

	for (uint8_t i = 0; i < n; i++)
	{
		data_point->channel[i] = ADS_Parse24Bit(&rx[ADS_STATUS_BYTES + i * ADS_BYTES_PER_CH]);
	}
}

void ADS_ReadAllChannels(ads1299_dev_t *dev, ads_data_point_t* data_point)
{
	ADS_ReadFirstNChannels(dev, ADS_NUM_CHANNELS, data_point);
}

void ADS_ReadDataOnce(ads1299_dev_t *dev, ads_data_point_t *data_point)
{
    uint8_t cmd = ADS_RDATA;
    uint8_t tx[ADS_FRAME_BYTES] = {0};
    uint8_t rx[ADS_FRAME_BYTES] = {0};

    ADS_CS_LOW(dev);

    HAL_SPI_Transmit(dev->spi_handle, &cmd, 1, HAL_MAX_DELAY);

    HAL_SPI_TransmitReceive(dev->spi_handle,
                            tx,
                            rx,
                            ADS_FRAME_BYTES,
                            HAL_MAX_DELAY);

    ADS_CS_HIGH(dev);

    data_point->timestamp = xTaskGetTickCount();

    data_point->status[0] = rx[0];
    data_point->status[1] = rx[1];
    data_point->status[2] = rx[2];

    for (uint8_t i = 0; i < ADS_NUM_CHANNELS; i++)
    {
        data_point->channel[i] = ADS_Parse24Bit(&rx[ADS_STATUS_BYTES + i * ADS_BYTES_PER_CH]);
    }
}


static int32_t ADS_Parse24Bit(const uint8_t *buf)
{
    int32_t value = ((int32_t)buf[0] << 16) |
                    ((int32_t)buf[1] << 8)  |
                    ((int32_t)buf[2]);

    if (value & 0x800000)
    {
        value |= 0xFF000000; // signed extension from 24 to 32 bits
    }

    return value;
}
