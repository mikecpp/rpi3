#ifndef __RPI_SPI_H__
#define __RPI_SPI_H__

#include "spi.h"

int32_t rpi_spi_init(uint8_t id);
int32_t rpi_spi_write(uint8_t id, uint8_t *pData, uint16_t len);
int32_t rpi_spi_read(uint8_t id, uint8_t *pData, uint16_t len);
int32_t rpi_spi_write_read(uint8_t id, uint8_t *pWrite, uint8_t *pRead, uint16_t len);

extern SPI_Driver_T rpi_spi_drv;

#endif//__RPI_SPI_H__
