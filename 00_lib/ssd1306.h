#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stdio.h>
#include <stdint.h>
#include "i2c.h"
#include "string.h"
#include "util.h"
#include "gui.h"

#define SSD1306_BLACK       0
#define SSD1306_WHITE       1
#define SSD1306_INVERSE     2

int32_t ssd1306_init(void);
int32_t ssd1306_width(void);
int32_t ssd1306_height(void);

int32_t ssd1306_read_pixel(uint16_t x, uint16_t y, uint32_t *color); 
int32_t ssd1306_draw_pixel(uint16_t x, uint16_t y, uint32_t color); 
int32_t ssd1306_clear(uint32_t color);
int32_t ssd1306_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
int32_t ssd1306_display(void);

void ssd1306_putc(uint8_t x, uint8_t y, char c);
void ssd1306_puts(uint8_t x, uint8_t y, char* str);

extern GUI_Interface_T ssd1306_gui_interface;

#endif//__SSD1306_H__
