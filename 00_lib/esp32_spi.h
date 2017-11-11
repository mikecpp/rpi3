#ifndef __ESP32_SPI_H__
#define __ESP32_SPI_H__

#include "spi.h"
#include "driver/spi_master.h"

int32_t esp32_spi_init(uint8_t id);
int32_t esp32_spi_write(uint8_t id, uint8_t *pData, uint16_t len);
int32_t esp32_spi_read(uint8_t id, uint8_t *pData, uint16_t len);
int32_t esp32_spi_write_read(uint8_t id, uint8_t *pWrite, uint8_t *pRead, uint16_t len);

extern SPI_Driver_T esp32_spi_drv;

#endif//__ESP32F103_SPI_H__
