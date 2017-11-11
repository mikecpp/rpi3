#ifndef __ESP32_I2C_H__
#define __ESP32_I2C_H__

#include "i2c.h"

int32_t esp32_i2c_init(uint8_t id);
int32_t esp32_i2c_write_byte(uint8_t id, uint8_t addr, uint8_t value);
int32_t esp32_i2c_read_byte(uint8_t id, uint8_t addr, uint8_t *value);
int32_t esp32_i2c_write(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);
int32_t esp32_i2c_read(uint8_t id, uint8_t addr, uint8_t *buf, int32_t len);

extern I2C_Driver_T esp32_i2c_drv;

#endif//__STM32_I2C_H__
