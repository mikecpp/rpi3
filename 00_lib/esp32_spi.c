#include "esp32_spi.h"
#include "esp_log.h"

#define PIN_NUM_MISO        25
#define PIN_NUM_MOSI        23
#define PIN_NUM_CLK         19
#define PIN_NUM_CS          22

#define TAG     "ESP32_SPI"

spi_device_handle_t m_spi[2]; 

SPI_Driver_T esp32_spi_drv = 
{
    .spi_init       = esp32_spi_init,
    .spi_write      = esp32_spi_write,
    .spi_read       = esp32_spi_read,
    .spi_write_read = esp32_spi_write_read,
};

int32_t esp32_spi_init(uint8_t id)
{   
    spi_bus_config_t buscfg= {
        .miso_io_num    = PIN_NUM_MISO,
        .mosi_io_num    = PIN_NUM_MOSI,
        .sclk_io_num    = PIN_NUM_CLK,
        .quadwp_io_num  = -1,
        .quadhd_io_num  = -1
    };
    
    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 20000000,
        .mode           = 0,                       
        .spics_io_num   = PIN_NUM_CS,              
        .queue_size     = 7,                        
    };
    
    if(spi_bus_initialize(HSPI_HOST, &buscfg, 1) != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize() fail !");
        return -1;
    }    
    
    if(spi_bus_add_device(HSPI_HOST, &devcfg, &m_spi[id]) != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device() fail !");
        return -2;
    }    

    ESP_LOGI(TAG, "spi_init() OK !");
    
    return 0;
}

int32_t esp32_spi_write(uint8_t id, uint8_t *pData, uint16_t len)
{
    spi_transaction_t t;
    
    t.flags     = 0;
    t.length    = len * 8;
    t.rxlength  = 0;
    t.tx_buffer = pData;
    t.rx_buffer = pData;
    
    if(spi_device_transmit(m_spi[id], &t) != ESP_OK) {
        ESP_LOGE(TAG, "spi_device_transmit() fail !");           
        return -1;
    }    
    
    return 0;
}

int32_t esp32_spi_read(uint8_t id, uint8_t *pData, uint16_t len)
{
    spi_transaction_t t;
    
    t.flags     = 0;
    t.length    = 0;
    t.rxlength  = len * 8;
    t.tx_buffer = pData;
    t.rx_buffer = pData;
    
    if(spi_device_transmit(m_spi[id], &t) != ESP_OK) {
        ESP_LOGE(TAG, "spi_device_transmit() fail !");           
        return -1;
    }    
    
    return 0;
}

int32_t esp32_spi_write_read(uint8_t id, uint8_t *pWrite, uint8_t *pRead, uint16_t len)
{
    spi_transaction_t t;
    
    t.flags     = 0;
    t.length    = len * 8;
    t.rxlength  = len * 8;
    t.tx_buffer = pWrite;
    t.rx_buffer = pRead;
    
    if(spi_device_transmit(m_spi[id], &t) != ESP_OK) {
        ESP_LOGE(TAG, "spi_device_transmit() fail !");       
        return -1;
    }    
    
    return 0;
}
