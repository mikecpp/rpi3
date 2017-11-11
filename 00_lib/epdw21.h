#ifndef __EPDW21_H__
#define __EPDW21_H__

#include <stdio.h>
#include <stdint.h>
#include "soc/gpio_struct.h"
#include "driver/gpio.h" 
#include "util.h"
#include "spi.h"
#include "gui.h"
#include "string.h"

int32_t epdw21_init(void);
int32_t epdw21_set_mode(uint8_t mode);

int32_t epdw21_get_width(void);
int32_t epdw21_get_height(void);

int32_t epdw21_clear(uint32_t color);
int32_t epdw21_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int32_t epdw21_read_pixel(uint16_t x, uint16_t y, uint32_t *Color);
int32_t epdw21_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
int32_t epdw21_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
int32_t epdw21_fill(uint8_t *data);
int32_t epdw21_update(void);

void SetFrameMemory(const uint8_t *image_buffer);
void ClearFrameMemory(uint8_t color);
void DisplayFrame(void);

extern GUI_Interface_T epdw21_gui_interface;

#endif // __EPDW21_H__
