#include <stdio.h>
#include <bcm2835.h>
#include "rpi_spi.h"

#define PIN_NUM_MISO        25
#define PIN_NUM_MOSI        23
#define PIN_NUM_CLK         19
#define PIN_NUM_CS          22

#define TAG     "rpi_SPI"

SPI_Driver_T rpi_spi_drv = 
{
    .spi_init       = rpi_spi_init,
    .spi_write      = rpi_spi_write,
    .spi_read       = rpi_spi_read,
    .spi_write_read = rpi_spi_write_read,
};

int32_t rpi_spi_init(uint8_t id)
{
    if(!bcm2835_init()) {
        printf("bcm2835_init failed. Are you running as root??\n");
        return -1;
    }
    
    if(!bcm2835_spi_begin()) {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return -1;
    }
    
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_4); 

    return 0;
}

int32_t rpi_spi_write(uint8_t id, uint8_t *pData, uint16_t len)
{
    bcm2835_spi_writenb(pData, len);    
    
    return 0;
}

int32_t rpi_spi_read(uint8_t id, uint8_t *pData, uint16_t len)
{
    bcm2835_spi_transfern(pData, len);
    
    return 0;
}

int32_t rpi_spi_write_read(uint8_t id, uint8_t *pWrite, uint8_t *pRead, uint16_t len)
{
    bcm2835_spi_transfernb(pWrite, pRead, len);

    return 0;
}
