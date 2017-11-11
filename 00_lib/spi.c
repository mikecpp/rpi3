#include "spi.h"

SPI_Driver_T *m_spi_drv = NULL;

int32_t spi_register_driver(SPI_Driver_T *spi_driver)
{
    m_spi_drv = spi_driver;

    return 0;
}

int32_t spi_init(uint8_t id)
{
    if(m_spi_drv == NULL) {
        return -1;
    }
        
    return m_spi_drv->spi_init(id);
}


int32_t spi_write(uint8_t id, uint8_t *pData, uint16_t len)
{    
    if(m_spi_drv == NULL) {
        return -1;
    }

    return m_spi_drv->spi_write(id, pData, len);
}

int32_t spi_read(uint8_t id, uint8_t *pData, uint16_t len)
{    
    if(m_spi_drv == NULL) {
        return -1;
    }

    return m_spi_drv->spi_read(id, pData, len);
}

int32_t spi_write_read(uint8_t id, uint8_t *pWrite, uint8_t *pRead, uint16_t len)
{   
    if(m_spi_drv == NULL) {
        return -1;
    }

    return m_spi_drv->spi_write_read(id, pWrite, pRead, len);
}