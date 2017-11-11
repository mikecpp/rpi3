#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

typedef struct {
    int32_t (*i2c_init)(uint8_t id);
    int32_t (*i2c_write_byte)(uint8_t id, uint8_t addr, uint8_t value); 
    int32_t (*i2c_read_byte)(uint8_t id, uint8_t addr, uint8_t *value); 
    int32_t (*i2c_write)(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);
    int32_t (*i2c_read)(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);
} I2C_Driver_T; 

typedef enum {
    I2C_1,
    I2C_2
} I2C_ID;

int32_t i2c_register_driver(I2C_Driver_T *i2c_driver);
int32_t i2c_init(uint8_t id); 
int32_t i2c_write_byte(uint8_t id, uint8_t addr, uint8_t value); 
int32_t i2c_read_byte(uint8_t id, uint8_t addr, uint8_t *value); 
int32_t i2c_write(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);
int32_t i2c_read(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);
int32_t i2c_write_reg(uint8_t id, uint8_t addr, uint8_t reg, uint8_t value); 
int32_t i2c_read_reg(uint8_t id, uint8_t addr, uint8_t reg, uint8_t *value);
int32_t i2c_write_reg_word(uint8_t id, uint8_t addr, uint8_t reg, uint16_t value); 
int32_t i2c_read_reg_word(uint8_t id, uint8_t addr, uint8_t reg, uint16_t *value);

#endif//_I2C_STM32F4_H__
