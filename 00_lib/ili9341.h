#ifndef __ILI9341_H__
#define __ILI9341_H__

#include <stdint.h>
#include "spi.h"
#include "gui.h"

#define RED		0xF800
#define GREEN	0x07E0
#define BLUE	0x001F
#define YELLOW  0xFFE0
#define CYAN	0x07FF
#define	PURPLE  0xF81F
#define BLACK	0x0000
#define WHITE	0xFFFF

#define ILI9341_PORTRAIT    1
#define ILI9341_LANDSCAPE   2

int32_t ili9341_init(void);
int32_t ili9341_set_mode(uint8_t mode);

int32_t ili9341_get_width(void);
int32_t ili9341_get_height(void);

int32_t ili9341_clear(uint32_t color);
int32_t ili9341_write_pixel(uint16_t x, uint16_t y, uint32_t color);
int32_t ili9341_read_pixel(uint16_t x, uint16_t y, uint32_t *Color);
int32_t ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
int32_t ili9341_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);

extern GUI_Interface_T ili9341_gui_interface;

#endif // __ILI9341_H__
