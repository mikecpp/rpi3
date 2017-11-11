#ifndef __MZTX06A_H__
#define __MZTX06A_H__

#include <stdint.h>
#include "soc/gpio_struct.h"
#include "driver/gpio.h" 
#include "util.h"
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

// #define LANDSCAPE

int32_t mztx06a_init(void);

int32_t mztx06a_get_width(void);
int32_t mztx06a_get_height(void);

int32_t mztx06a_clear(uint32_t color);
int32_t mztx06a_get_pixel(uint16_t dx, uint16_t dy, uint32_t *color);
int32_t mztx06a_draw_pixel(uint16_t dx, uint16_t dy, uint32_t color);
int32_t mztx06a_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);

extern GUI_Interface_T mztx06a_gui_interface;

#endif // __MZTX06A_H__
