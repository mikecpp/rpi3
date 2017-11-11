#include <stdio.h>
#include "i2c.h" 

I2C_Driver_T *m_i2c_drv = NULL;

int32_t i2c_register_driver(I2C_Driver_T *i2c_driver)
{
    m_i2c_drv = i2c_driver;
    
    return 0;
}

int32_t i2c_init(uint8_t id)
{
    if(m_i2c_drv == NULL) {
        return -1;
    }    
    
    return m_i2c_drv->i2c_init(id);
}

int32_t i2c_write_byte(uint8_t id, uint8_t addr, uint8_t value)
{ 
    if(m_i2c_drv == NULL) {
        return -1;
    }

    return m_i2c_drv->i2c_write_byte(id, addr, value);
}

int32_t i2c_read_byte(uint8_t id, uint8_t addr, uint8_t *value)
{
    if(m_i2c_drv == NULL) {
        return -1;
    }

    return m_i2c_drv->i2c_read_byte(id, addr, value);
}

int32_t i2c_write(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len)
{
    if(m_i2c_drv == NULL) {
        return -1;
    }
    
    return m_i2c_drv->i2c_write(id, addr, buf, len); 
}

int32_t i2c_read(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len) 
{
    if(m_i2c_drv == NULL) {
        return -1;
    }
    
    return m_i2c_drv->i2c_read(id, addr, buf, len);    
}

int32_t i2c_write_reg(uint8_t id, uint8_t addr, uint8_t reg, uint8_t value)
{
    uint32_t ret_code;
    uint8_t buf[2];
    
    buf[0] = reg;
    buf[1] = value;
    
    ret_code = i2c_write(id, addr, (uint8_t*) buf, 2);
    
    return ret_code;
}

int32_t i2c_read_reg(uint8_t id, uint8_t addr, uint8_t reg, uint8_t *value)
{
    uint32_t ret_code;
    
    ret_code = i2c_write_byte(id, addr, reg); 
    ret_code = i2c_read_byte(id, addr, value);
    
    return ret_code;
}

int32_t i2c_write_reg_word(uint8_t id, uint8_t addr, uint8_t reg, uint16_t value)
{
    uint32_t ret_code;
    uint8_t buf[3];
    
    buf[0] = reg;
    buf[1] = (value & 0xFF00) >> 8;
    buf[2] = value & 0x00FF;
    
    ret_code = i2c_write(id, addr, buf, 3);
    
    return ret_code;
}

int32_t i2c_read_reg_word(uint8_t id, uint8_t addr, uint8_t reg, uint16_t *value)
{
    uint32_t ret_code;
    uint8_t buf[2];
    
    ret_code = i2c_write_byte(id, addr, reg);   
    ret_code = i2c_read(id, addr, buf, 2);
    
    *value  = buf[1] << 8;  // HIGH BYTE
    *value += buf[0];       // LOW  BYTE
    
    return ret_code;
}
