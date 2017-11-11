#include <stdio.h>
#include "driver/i2c.h"
#include "i2c.h"
#include "esp32_i2c.h"

#define I2C_MAX_NUM         2

typedef struct{
    uint8_t     i2c_num;
    uint8_t     scl_pin;
    uint8_t     sda_pin;
    uint32_t    freq;
} I2C_Handle_T;    

I2C_Handle_T m_i2c_handle[I2C_MAX_NUM] =
{
    {I2C_NUM_0, 18, 19, 100000},
    {I2C_NUM_1, 25, 26, 100000}
};        

I2C_Driver_T esp32_i2c_drv = 
{
    esp32_i2c_init,
    esp32_i2c_write_byte,
    esp32_i2c_read_byte,
    esp32_i2c_write,
    esp32_i2c_read
};

int32_t esp32_i2c_init(uint8_t id)
{
    i2c_config_t conf;

    conf.mode               = I2C_MODE_MASTER;
    conf.scl_io_num         = m_i2c_handle[id].scl_pin;
    conf.sda_io_num         = m_i2c_handle[id].sda_pin;
    conf.master.clk_speed   = m_i2c_handle[id].freq;
    conf.scl_pullup_en      = GPIO_PULLUP_ENABLE;
    conf.sda_pullup_en      = GPIO_PULLUP_ENABLE;
    
    i2c_param_config(m_i2c_handle[id].i2c_num, &conf);
    i2c_driver_install(m_i2c_handle[id].i2c_num, conf.mode, 0, 0, 0);    
    
    return 0;
}

int32_t esp32_i2c_write_byte(uint8_t id, uint8_t addr, uint8_t value)
{
    return esp32_i2c_write(id, addr, &value, 1);
}

int32_t esp32_i2c_read_byte(uint8_t id, uint8_t addr, uint8_t *value)
{
    return esp32_i2c_read(id, addr, value, 1);
}

int32_t esp32_i2c_write(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( addr << 1 ) | I2C_MASTER_WRITE, 0x01); // ACK_CHECK_EN
    i2c_master_write(cmd, buf, len, 0x01); // ACK_CHECK_EN
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(m_i2c_handle[id].i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);  
    
    return ret; 
}

int32_t esp32_i2c_read(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( addr << 1 ) | I2C_MASTER_READ, 0x01);
    
    if (len > 1) {
        i2c_master_read(cmd, buf, len - 1, 0x00); // ACK
    }
    
    i2c_master_read_byte(cmd, buf + len - 1, 0x01); // NACK
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(m_i2c_handle[id].i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);    
    
    return ret;    
}
