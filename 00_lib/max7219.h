#ifndef __MAX7219_H__
#define __MAX7219_H__

#include "spi.h"
#include "util.h"

int max7219_init(SPI_ID id);
void max7219_write(uint8_t addr, uint8_t data);
void max7219_intensity(uint8_t value);
void max7219_test(uint8_t enable);
void max7219_pixel(uint8_t x, uint8_t y, uint8_t enable);
void max7219_char(char ch);
void max7219_scroll_char(char ch);
void max7219_scroll_string(char *str);

#endif//__MAX7219_H__
