#include "ssd1306.h" 
#include "esp_log.h"

#define SSD1306_ADDR        0x3C
#define sleep(val)          delay_ms(val)
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

#define FONT_8_16

#ifdef FONT_8_16
    #include "font_8_16.h"
    #define FONT_WIDTH       8
    #define FONT_HEIGHT     16
#else
    #include "font_16_24.h"
    #define FONT_WIDTH      16 
    #define FONT_HEIGHT     24
#endif

#define TAG     "SSD1306"

GUI_Interface_T ssd1306_gui_interface = 
{
    .GUI_Init           = ssd1306_init, 
    .GUI_GetXSize       = ssd1306_width,
    .GUI_GetYSize       = ssd1306_height,
    .GUI_ReadPixel      = ssd1306_read_pixel,
    .GUI_DrawPixel      = ssd1306_draw_pixel,
    .GUI_Clear          = ssd1306_clear,
    .GUI_DrawRGBImage   = ssd1306_bmp,
    .GUI_Update         = ssd1306_display,
};

// 128 x 64 
static uint8_t _disp_buf[1024];
static char m_i2c_id = 0xFF;

int ssd1306_command(char value)
{
    int ret = 0;
    uint8_t buf[2];
    
    buf[0] = 0x00;
    buf[1] = value;
    ret = i2c_write(m_i2c_id, SSD1306_ADDR, buf, 2);
    
    return ret;
}

int ssd1306_command_2(char v1, char v2)
{
    int ret = 0;
    uint8_t buf[3];
    
    buf[0] = 0x00;
    buf[1] = v1;
    buf[2] = v2;
    ret = i2c_write(m_i2c_id, SSD1306_ADDR, buf, 3);
    
    return ret;
}

int ssd1306_data(char value)
{   
    int ret = 0;
    uint8_t buf[2];
    
    buf[0] = 0x40;
    buf[1] = value;
    ret = i2c_write(m_i2c_id, SSD1306_ADDR, buf, 2);
    
    return ret;
}

int32_t ssd1306_init(void)
{
    uint8_t buf[4];
    
    m_i2c_id = 0;
    
    if(i2c_init(m_i2c_id) != 0) {
        printf("I2C init fail !\r\n");
        return -1;
    }   
    
    sleep(1000);    
    
    // Init sequence for 128x64 OLED module 
    ssd1306_command(0xAE);                    // Display Off
    ssd1306_command_2(0xD5, 0x80);            // Set Clock DIV 
    ssd1306_command_2(0xA8, 0x3F);            // MultiPlex 
    ssd1306_command_2(0xD3, 0x00);            // Display Offset
    ssd1306_command(0x40 | 0x00);             // Set Start Line
    ssd1306_command_2(0x8D, 0x14);            // Charge Pump
    ssd1306_command_2(0x20, 0x00);            // Memory Mode
    ssd1306_command(0xA0 | 0x1);              // rotate screen 180 
    ssd1306_command(0xC8);                    // COMSCANDEC
    ssd1306_command_2(0xDA, 0x12);            // Set COMPINS
    ssd1306_command_2(0x81, 0x9F);            // Set Contrast
    ssd1306_command_2(0xD9, 0xF1);            // Step Recharge
    ssd1306_command_2(0xDB, 0x40);            // Set VCOM Detect

    ssd1306_command(0x2E);                    
    ssd1306_command(0xA4);                    // Display All On Resume
    ssd1306_command(0xA6);                    // Normal Display
    
    // Reset OLED
    buf[0] = 0x00;
    buf[1] = 0x21;
    buf[2] = 0x00;
    buf[3] = 0x7F;    
    i2c_write(m_i2c_id, SSD1306_ADDR, (uint8_t*) buf, 4);

    buf[1] = 0x22;
    buf[3] = 0x07;
    i2c_write(m_i2c_id, SSD1306_ADDR, (uint8_t*) buf, 4);
    
    ssd1306_command(0x2E);                    // Stop Scroll   
    ssd1306_command(0xAF);                    // Switch on OLED     
    
    ssd1306_clear(SSD1306_BLACK);
    
    return 0;
}

int32_t ssd1306_width(void)
{
    return SSD1306_WIDTH;
}

int32_t ssd1306_height(void)
{
    return SSD1306_HEIGHT;
}

static uint8_t _buf[17];
int32_t ssd1306_display(void)
{  
    uint16_t i=0;
    uint8_t  x;
    uint8_t  *p = _disp_buf;
     
    ssd1306_command(0x00 | 0x00); // Low Col   = 0
    ssd1306_command(0x10 | 0x00); // Hight Col = 0
    ssd1306_command(0x40 | 0x00); // Line #0
    
    for(i=0; i<(SSD1306_WIDTH*SSD1306_HEIGHT/8); i+=16)
    {
        _buf[0] = 0x40;
        for(x=1; x<=16; x++)
        {
            _buf[x] = *p++;
        }    
        i2c_write(m_i2c_id, SSD1306_ADDR, _buf, 17);
    }
    
    return 0;
}

int32_t ssd1306_clear(uint32_t color)
{
    uint8_t c = (uint8_t) color;
    
    if(c == SSD1306_BLACK) {
        memset(_disp_buf, 0x00, 1024);
    }
    else {
        memset(_disp_buf, 0xFF, 1024);
    }   

    return 0;
}

int32_t ssd1306_read_pixel(uint16_t x, uint16_t y, uint32_t *color)
{   
    return 0;
}

int32_t ssd1306_draw_pixel(uint16_t x, uint16_t y, uint32_t color)
{    
    uint8_t c = (uint8_t) color;

    switch (c)
    {
      case SSD1306_WHITE:   
          _disp_buf[x + (y/8) * SSD1306_WIDTH] |=  (1 << (y & 7)); 
          break;
      
      case SSD1306_BLACK:   
          _disp_buf[x + (y/8) * SSD1306_WIDTH] &= ~(1 << (y & 7)); 
          break;
      
      case SSD1306_INVERSE: 
          _disp_buf[x + (y/8) * SSD1306_WIDTH] ^=  (1 << (y & 7));  
          break;
    }    
    
    return 0;
}

int32_t ssd1306_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
    uint8_t i, j;
    int index = 0;
    
    for(i=0; i<=h; i++)
    {
        for(j=0; j<=w; j++)
        {
            if(data[index] == SSD1306_BLACK) {
                ssd1306_draw_pixel(x+j, y+i, SSD1306_BLACK);
            }
            else {
                ssd1306_draw_pixel(x+j, y+i, SSD1306_WHITE);
            }
            index += 2;
        }
    }
    
    return 0;
}

void ssd1306_putc(uint8_t x, uint8_t y, char c)
{
    char i, j;
    uint8_t *p = (uint8_t *) &ascii[c * FONT_HEIGHT];  

    for(i=0; i<FONT_HEIGHT; i++)
    {
        for(j=0; j<FONT_WIDTH; j++)
        {
            ssd1306_draw_pixel(x+j, y+i, SSD1306_BLACK);
            if((*p << j) & 0x80) 
            {
                ssd1306_draw_pixel(x+j, y+i, SSD1306_WHITE);
            }    
        }
        p++;
    }
    
}

void ssd1306_puts(uint8_t x, uint8_t y, char* str)
{
    uint16_t i;
    char *p = str;
    
    for(i=0; i<strlen(str); i++)
    {
        ssd1306_putc(x+(i*8), y, *p++);
    }
}
