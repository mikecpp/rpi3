#ifndef __OLED1602_H__
#define __OLED1602_H__

#include <stdbool.h>
#include "i2c.h"

int oled1602_init(uint8_t i2c_id);
int oled1602_puts(char *s);
int oled1602_clear(void);
int oled1602_cursor(uint8_t col, uint8_t row);
int oled1602_backlight(bool enable);
void oled1602_on(void);
void oled1602_off(void);
void oled1602_display(void);

#endif//__LCM_1602_H__
