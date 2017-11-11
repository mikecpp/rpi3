#include <bcm2835.h>
#include <stdio.h>

#include "ili9341.h"

uint16_t ILI9341_WIDTH  = 240;
uint16_t ILI9341_HEIGHT = 320;

#define	SPICS	RPI_V2_GPIO_P1_24	// GPIO08
#define	SPIRS	RPI_V2_GPIO_P1_22	// GPIO25
#define	SPIRST	RPI_V2_GPIO_P1_16	// GPIO09
#define	SPISCL	RPI_V2_GPIO_P1_23	// GPIO11
#define	SPISCI	RPI_V2_GPIO_P1_19	// GPIO10
#define	LCDPWM  RPI_V2_GPIO_P1_12	// GPIO18

#define LCD_CS_CLR      bcm2835_gpio_write(SPICS,  LOW)
#define LCD_CS_SET      bcm2835_gpio_write(SPICS,  HIGH)
#define LCD_RS_CLR      bcm2835_gpio_write(SPIRS,  LOW)
#define LCD_RS_SET      bcm2835_gpio_write(SPIRS,  HIGH)
#define LCD_RST_CLR     bcm2835_gpio_write(SPIRST, LOW)
#define LCD_RST_SET     bcm2835_gpio_write(SPIRST, HIGH)

GUI_Interface_T ili9341_gui_interface = 
{
    .GUI_Init           = ili9341_init, 
    .GUI_GetXSize       = ili9341_get_width,
    .GUI_GetYSize       = ili9341_get_height,
    .GUI_ReadPixel      = ili9341_read_pixel,
    .GUI_DrawPixel      = ili9341_write_pixel,
    .GUI_Clear          = ili9341_clear,
    .GUI_DrawRGBImage   = ili9341_bmp
};
 
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; 
} lcd_init_cmd_t;

static const lcd_init_cmd_t init_cmds[]={
    {0xEF, {0x03, 0x80, 0x02}, 3},
    {0xCF, {0x00, 0xC1, 0X30}, 3},
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    {0xE8, {0x85, 0x00, 0x78}, 3},
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    {0xF7, {0x20}, 1},
    {0xEA, {0x00, 0x00}, 2},
    {0xC0, {0x23}, 1},
    {0xC1, {0x10}, 1},
    {0xC5, {0x3E, 0x28}, 2},
    {0xC7, {0x86}, 1},
    {0x36, {0x48}, 1},
    {0x3A, {0x55}, 1},
    {0xB1, {0x00, 0x18}, 2},
    {0xB6, {0x08, 0x82, 0x27}, 3},
    {0xF2, {0x00}, 1},
    {0x26, {0x01}, 1},
    {0xE0, {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15},
    {0XE1, {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15},
    {0, {0}, 0xff},
};

static uint16_t RGB888to565(uint32_t Color)
{
    uint8_t *ptr = (uint8_t*) &Color;
    
    uint8_t red   = ptr[2];
    uint8_t green = ptr[1];
    uint8_t blue  = ptr[0];
    
    uint16_t b = (blue >> 3)   & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3)   & 0x1f) << 11;
    
    return (uint16_t) (r | g | b);    
}

static void ili9341_cmd(uint8_t cmd)
{
    LCD_CS_CLR;
    LCD_RS_CLR;
	spi_write(0, &cmd, 1);
    LCD_RS_SET;
    LCD_CS_SET;
}

static void ili9341_data(uint8_t *data, uint32_t len)
{
    LCD_CS_CLR;    
    LCD_RS_SET;
    spi_write(0, data, len);
    LCD_RS_CLR;
    LCD_CS_SET;    
}

static void ili9341_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];
    
    ili9341_cmd(0x2A);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    ili9341_data((uint8_t *) data, 4); 
    
    ili9341_cmd(0x2B);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    ili9341_data((uint8_t *) data, 4); 
    
    ili9341_cmd(0x2C);
}

//////////////////////////////////////////////////////////////////////////////
// Public API

int32_t ili9341_init(void)
{
    spi_init(0);
    
    // Setup GPIO PIN to output
    bcm2835_gpio_fsel(SPICS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SPIRS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SPIRST, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LCDPWM, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(LCDPWM, HIGH);    
       
	LCD_RST_CLR;
	delay(20);	
	LCD_RST_SET;
	delay(20);   

    int i = 0;
    while(init_cmds[i].cmd != 0x00) {
        ili9341_cmd(init_cmds[i].cmd);
        ili9341_data((uint8_t *) init_cmds[i].data, init_cmds[i].databytes);
        if(init_cmds[i].databytes & 0x80) {
            delay(100);
        }
        i++;
    }
   
    ili9341_cmd(0x11);
    delay(120);
    
    ili9341_cmd(0x29);
    ili9341_cmd(0x2C);
   
    return 0;
}

int32_t ili9341_set_mode(uint8_t mode)
{
    uint8_t data;

    ili9341_cmd(0x36);
    
    if(mode == ILI9341_LANDSCAPE) { 
        data = 0xE8;  
        ILI9341_WIDTH  = 320;
        ILI9341_HEIGHT = 240;        
    }
    else {
        data = 0x48;
        ILI9341_WIDTH  = 240;
        ILI9341_HEIGHT = 320;        
    }

    ili9341_data(&data, 1);
    
    return 0;
}

int32_t ili9341_get_width(void) 
{
    return ILI9341_WIDTH;
}

int32_t ili9341_get_height(void)
{
    return ILI9341_HEIGHT;
}

#define BUF_SIZE    512

static void ili9341_fill_color(uint16_t color, int32_t len)
{
    char data[BUF_SIZE * 2];
    int  i;
    
    while(len > 0) {
        for(i=0; i<BUF_SIZE; i++) {
            data[i*2]   = color >> 8;
            data[i*2+1] = color & 0xFF;
        }
        ili9341_data((uint8_t *) data, (len >= BUF_SIZE) ? (BUF_SIZE * 2) : (len * 2)); 
        len -= BUF_SIZE;
    }     
}

int32_t ili9341_clear(uint32_t color)
{
    ili9341_addr_window(0, 0, ILI9341_WIDTH-1, ILI9341_HEIGHT-1);
    ili9341_fill_color(color, ILI9341_WIDTH * ILI9341_HEIGHT);
    
    return 0;
}

int32_t ili9341_write_pixel(uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t data[2];
    uint16_t c = RGB888to565(color);
       
    ili9341_addr_window(x, y, x+1, y+1);
    
    data[0] = c >> 8;
    data[1] = c & 0xFF;
    ili9341_data((uint8_t *) data, 2);
    
    return 0;
}

int32_t ili9341_read_pixel(uint16_t x, uint16_t y, uint32_t *color)
{
    return 0;
}

int32_t ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    int len = w * h;
    
    ili9341_addr_window(x, y, x+w, y+h);
    
    char data[BUF_SIZE * 2];
    int  i;
    
    while(len > 0) {
        for(i=0; i<BUF_SIZE; i++) {
            data[i*2]   = color >> 8;
            data[i*2+1] = color & 0xFF;
        }
        ili9341_data((uint8_t *) data, (len >= BUF_SIZE) ? (BUF_SIZE * 2) : (len * 2)); 
        len -= BUF_SIZE;
    }       
    
    return 0;
}

int32_t ili9341_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
    ili9341_addr_window(x, y, x+w, y+h);   
    ili9341_data(data, w * h * 2);     
    
    return 0;
}
