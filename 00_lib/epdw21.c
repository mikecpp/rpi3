#include "epdw21.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint16_t EPDW21_WIDTH  = 200;
uint16_t EPDW21_HEIGHT = 200;

#define TAG             "EPDW21"
#define PIN_RS		    21

#define LCD_RS_CLR      gpio_set_level(PIN_RS,  0)
#define LCD_RS_SET      gpio_set_level(PIN_RS,  1)

GUI_Interface_T epdw21_gui_interface = 
{
    .GUI_Init           = epdw21_init, 
    .GUI_GetXSize       = epdw21_get_width,
    .GUI_GetYSize       = epdw21_get_height,
    .GUI_ReadPixel      = epdw21_read_pixel,
    .GUI_DrawPixel      = epdw21_draw_pixel,
    .GUI_Clear          = epdw21_clear,
    .GUI_DrawRGBImage   = epdw21_bmp,
    .GUI_Update         = epdw21_update,
};
 
static void epdw21_cmd(uint8_t cmd)
{
    LCD_RS_CLR;
	spi_write(0, &cmd, 1);
    LCD_RS_SET;
}

static void epdw21_data(uint8_t data)
{
    LCD_RS_SET;
    spi_write(0, &data, 1);
    LCD_RS_CLR;
}

uint8_t lut_full[30] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69,
    0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

uint8_t lut_partial[30] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define BUF_SIZE        5000

static uint8_t _disp_buf[BUF_SIZE];

static void epdw21_write_LUT(uint8_t *data)
{   
    int i;

    epdw21_cmd(0x32);
    for(i = 0; i < 30; i++) {
        epdw21_data(*(data+i));
    }    
}

int32_t epdw21_fill(uint8_t *data)
{
    
    return 0;
}

static void SetMemoryPointer(int16_t x, int16_t y)
{
    epdw21_cmd(0x4E);
    epdw21_data((x >> 3) & 0xFF);
    
    epdw21_cmd(0x4F);
    epdw21_data(y & 0xFF);
    epdw21_data((y >> 8) & 0xFF);
    
    delay_ms(100);    
}

static void SetMemoryArea(int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end)
{
    epdw21_cmd(0x44);
    epdw21_data((x_start >> 3) & 0xFF);
    epdw21_data((x_end >> 3) & 0xFF);
    
    epdw21_cmd(0x45);
    epdw21_data(y_start & 0xFF);
    epdw21_data((y_start >> 8) & 0xFF);
    epdw21_data(y_end & 0xFF);
    epdw21_data((y_end >> 8) & 0xFF);    
    
    SetMemoryPointer(x_start, y_start);
}

void SetFrameMemory(const uint8_t *image_buffer)
{
    int i;
    
    SetMemoryArea(0, 0,  EPDW21_WIDTH - 1, EPDW21_HEIGHT - 1);
    epdw21_cmd(0x24);
    for (i = 0; i < EPDW21_WIDTH / 8 * EPDW21_HEIGHT; i++) {
        epdw21_data(image_buffer[i]);
    } 
}

void ClearFrameMemory(uint8_t color) 
{
    int i;
    
    SetMemoryArea(0, 0,  EPDW21_WIDTH - 1, EPDW21_HEIGHT - 1);
    epdw21_cmd(0x24);
    for (i = 0; i < EPDW21_WIDTH / 8 * EPDW21_HEIGHT; i++) {
        epdw21_data(color);
    } 
}

void DisplayFrame(void)
{
    epdw21_cmd(0x22);
    epdw21_data(0xC4);
    epdw21_cmd(0x20);
    epdw21_cmd(0xFF);
    delay_ms(100);
}

//////////////////////////////////////////////////////////////////////////////
// Public API

int32_t epdw21_init(void)
{
    gpio_set_direction(PIN_RS,  GPIO_MODE_OUTPUT);
       
    spi_init(0);

    // Driver Output Control
    epdw21_cmd(0x01);
    epdw21_data(0xC7);
    epdw21_data(0x00);
    epdw21_data(0x00);
    
    // Soft Start
    epdw21_cmd(0x0C);
    epdw21_data(0xD7);
    epdw21_data(0xD6);
    epdw21_data(0x9D);
    
    // VCOM 7C
    epdw21_cmd(0x2C);
    epdw21_data(0xA8);    
    
    // 4 Dummy Line
    epdw21_cmd(0x3A);
    epdw21_data(0x1A);    

    // 2us per line
    epdw21_cmd(0x3B);
    epdw21_data(0x08);    

    // Entry Mode 
    epdw21_cmd(0x11);
    epdw21_data(0x03);    

    epdw21_write_LUT(lut_full);
   
    ClearFrameMemory(0x00);  
    DisplayFrame();
    
    delay_ms(1000);   
   
    epdw21_write_LUT(lut_partial);   
    
    return 0;
}

int32_t epdw21_get_width(void) 
{
    return EPDW21_WIDTH;
}

int32_t epdw21_get_height(void)
{
    return EPDW21_HEIGHT;
}

int32_t epdw21_clear(uint32_t color)
{  
    uint8_t c = (uint8_t) color;

    if(c == 0x00) {
        memset(_disp_buf, 0x00, BUF_SIZE);
    }
    else {
        memset(_disp_buf, 0xFF, BUF_SIZE);
    }   

    return 0;
}

int32_t epdw21_draw_pixel(uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t c = (uint8_t) color;

    switch (c)
    {
      case 0x00:   
          _disp_buf[(x + y * EPDW21_WIDTH) / 8] &= ~(0x80 >> (x % 8));
          break;

      default:  
          _disp_buf[(x + y * EPDW21_WIDTH) / 8] |= 0x80 >> (x % 8);
          break;          
    }    
    
    return 0;
}

int32_t epdw21_read_pixel(uint16_t x, uint16_t y, uint32_t *color)
{
    return 0;
}

int32_t epdw21_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    
    return 0;
}

int32_t epdw21_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
    uint8_t i, j;
    int index = 0;
    
    for(i=0; i<=h; i++)
    {
        for(j=0; j<=w; j++)
        {
            if(data[index] == 0x00) {
                epdw21_draw_pixel(x+j, y+i, 0x00);
            }
            else {
                epdw21_draw_pixel(x+j, y+i, 0xFF);
            }
            index += 2;
        }
    }
    
    return 0;
}

int32_t epdw21_update(void)
{   
    SetFrameMemory(_disp_buf);
    DisplayFrame();

    return 0;
}
