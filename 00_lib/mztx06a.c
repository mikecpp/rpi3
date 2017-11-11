#include "mztx06a.h"
#include "esp_log.h"

uint32_t color[] = {RED, GREEN, BLUE, BLACK, WHITE, YELLOW, CYAN, PURPLE};

#ifdef  LANDSCAPE
#define XP    0x0201
#define YP    0x0200
#define XS    0x0212
#define YS    0x0210
#define XE    0x0213
#define YE    0x0211
#define MZTX06A_WIDTH     319
#define MZTX06A_HEIGHT    239
#else
#define XP    0x0200
#define YP    0x0201
#define XS    0x0210
#define YS    0x0212
#define XE    0x0211
#define YE    0x0213
#define MZTX06A_WIDTH     239
#define MZTX06A_HEIGHT    319
#endif

#define PIN_RS		    21
#define PIN_RST		    18
#define PIN_LED         05

#define BUF_SIZE        512

#define LCD_RS_CLR      gpio_set_level(PIN_RS,  0)
#define LCD_RS_SET      gpio_set_level(PIN_RS,  1)
#define LCD_RST_CLR     gpio_set_level(PIN_RST, 0)
#define LCD_RST_SET     gpio_set_level(PIN_RST, 1)

#define TAG     "MZTX06A"

GUI_Interface_T mztx06a_gui_interface = 
{
    .GUI_Init           = mztx06a_init, 
    .GUI_GetXSize       = mztx06a_get_width,
    .GUI_GetYSize       = mztx06a_get_height,
    .GUI_ReadPixel      = mztx06a_get_pixel,
    .GUI_DrawPixel      = mztx06a_draw_pixel,
    .GUI_Clear          = mztx06a_clear,
    .GUI_DrawRGBImage   = mztx06a_bmp,
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
 
static uint8_t data[2];
 
void Delay(uint32_t t)
{
    delay_ms(t);
}

static void mztx06a_cmd(uint16_t cmd)
{   
	LCD_RS_CLR; // Command
    
    data[0] = cmd >> 8;   
    data[1] = cmd;
    spi_write(0, (uint8_t*) data, 2);
}

static void mztx06a_cmd_data(uint16_t cmd, uint16_t val)
{
	LCD_RS_CLR;  // Command
    data[0] = cmd >> 8;   
    data[1] = cmd;
    spi_write(0, (uint8_t*) data, 2);
    
	LCD_RS_SET; // Data
    data[0] = val >> 8;   
    data[1] = val;
    spi_write(0, (uint8_t*) data, 2);   	
}

static void mztx06a_data(uint8_t *data, int32_t len)
{
	LCD_RS_SET;
    spi_write(0, data, len);   
}

static void mztx06a_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	mztx06a_cmd_data(XS, x0);
	mztx06a_cmd_data(YS, y0);
	mztx06a_cmd_data(XE, x1);
	mztx06a_cmd_data(YE, y1);
}

static void mztx06a_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    int len = w * h;
    
    mztx06a_addr_window(x, y, x+w, y+h);
    mztx06a_cmd_data(XP, x);
	mztx06a_cmd_data(YP, y);       
    mztx06a_cmd(0x202);
    
    char data[BUF_SIZE * 2];
    int  i;
    
    while(len > 0) {
        for(i=0; i<BUF_SIZE; i++) {
            data[i*2]   = color >> 8;
            data[i*2+1] = color & 0xFF;
        }
        mztx06a_data((uint8_t *) data, (len >= BUF_SIZE) ? (BUF_SIZE * 2) : (len * 2)); 
        len -= BUF_SIZE;
    }     
}

//////////////////////////////////////////////////////////////////////////////
// Public API

int32_t mztx06a_init(void)
{
    // Setup GPIO PIN to output
    gpio_set_direction(PIN_RS,  GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);    
    
    gpio_set_level(PIN_LED, 1); // Turn on LED
    
    spi_init(0);
    
	LCD_RST_CLR;
	Delay(100);	
	LCD_RST_SET;
	Delay(100);

	mztx06a_cmd_data( 0x000, 0x0001 ); /* oschilliation start */
	Delay ( 1 );
	/* Power settings */
	mztx06a_cmd_data( 0x100, 0x0000 ); /*power supply setup*/
	mztx06a_cmd_data( 0x101, 0x0000 );
	mztx06a_cmd_data( 0x102, 0x3110 );
	mztx06a_cmd_data( 0x103, 0xe200 );
	mztx06a_cmd_data( 0x110, 0x009d );
	mztx06a_cmd_data( 0x111, 0x0022 );
	mztx06a_cmd_data( 0x100, 0x0120 );
	Delay ( 2 );
    
	mztx06a_cmd_data( 0x100, 0x3120 );
	Delay ( 8 );
	/* Display control */
    
	mztx06a_cmd_data( 0x002, 0x0000 );
#ifdef	ROTATE90
	mztx06a_cmd_data( 0x001, 0x0000 );
	mztx06a_cmd_data( 0x003, 0x12B8 );
#else
	mztx06a_cmd_data( 0x001, 0x0100 );
	mztx06a_cmd_data( 0x003, 0x1230 );
#endif
	mztx06a_cmd_data( 0x006, 0x0000 );
	mztx06a_cmd_data( 0x007, 0x0101 );
	mztx06a_cmd_data( 0x008, 0x0808 );
	mztx06a_cmd_data( 0x009, 0x0000 );
	mztx06a_cmd_data( 0x00b, 0x0000 );
	mztx06a_cmd_data( 0x00c, 0x0000 );
	mztx06a_cmd_data( 0x00d, 0x0018 );
	/* LTPS control settings */
	mztx06a_cmd_data( 0x012, 0x0000 );
	mztx06a_cmd_data( 0x013, 0x0000 );
	mztx06a_cmd_data( 0x018, 0x0000 );
	mztx06a_cmd_data( 0x019, 0x0000 );
    
	mztx06a_cmd_data( 0x203, 0x0000 );
	mztx06a_cmd_data( 0x204, 0x0000 );
    
	mztx06a_cmd_data( 0x210, 0x0000 );
	mztx06a_cmd_data( 0x211, 0x00ef );
	mztx06a_cmd_data( 0x212, 0x0000 );
	mztx06a_cmd_data( 0x213, 0x013f );
	mztx06a_cmd_data( 0x214, 0x0000 );
	mztx06a_cmd_data( 0x215, 0x0000 );
	mztx06a_cmd_data( 0x216, 0x0000 );
	mztx06a_cmd_data( 0x217, 0x0000 );
    
	// Gray scale settings
	mztx06a_cmd_data( 0x300, 0x5343);
	mztx06a_cmd_data( 0x301, 0x1021);
	mztx06a_cmd_data( 0x302, 0x0003);
	mztx06a_cmd_data( 0x303, 0x0011);
	mztx06a_cmd_data( 0x304, 0x050a);
	mztx06a_cmd_data( 0x305, 0x4342);
	mztx06a_cmd_data( 0x306, 0x1100);
	mztx06a_cmd_data( 0x307, 0x0003);
	mztx06a_cmd_data( 0x308, 0x1201);
	mztx06a_cmd_data( 0x309, 0x050a);
    
	/* RAM access settings */
	mztx06a_cmd_data( 0x400, 0x4027 );
	mztx06a_cmd_data( 0x401, 0x0000 );
	mztx06a_cmd_data( 0x402, 0x0000 );	/* First screen drive position (1) */
	mztx06a_cmd_data( 0x403, 0x013f );	/* First screen drive position (2) */
	mztx06a_cmd_data( 0x404, 0x0000 );
    
	mztx06a_cmd_data( 0x200, 0x0000 );
	mztx06a_cmd_data( 0x201, 0x0000 );
    
	mztx06a_cmd_data( 0x100, 0x7120 );
	mztx06a_cmd_data( 0x007, 0x0103 );
	Delay( 1 );
	mztx06a_cmd_data( 0x007, 0x0113 );
    
    return 0;
}

int32_t mztx06a_clear(uint32_t color)
{
    uint16_t c = RGB888to565(color);
    
    mztx06a_fill_rect(0, 0, MZTX06A_WIDTH+1, MZTX06A_HEIGHT+1, c);
    
    return 0;
}

int32_t mztx06a_get_width(void)
{
    return MZTX06A_WIDTH;
}

int32_t mztx06a_get_height(void)
{
    return MZTX06A_HEIGHT;
}

int32_t mztx06a_get_pixel(uint16_t dx, uint16_t dy, uint32_t *color)
{
    return 0;
}

int32_t mztx06a_draw_pixel(uint16_t dx, uint16_t dy, uint32_t color)
{
    uint16_t c = RGB888to565(color);
    
    mztx06a_addr_window(dx, dy, dx, dy);
    mztx06a_cmd_data(XP, dx);
	mztx06a_cmd_data(YP, dy);   
    mztx06a_cmd_data(0x202, c);
    
    return 0;
}

int32_t mztx06a_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
    mztx06a_addr_window(x, y, x+w, y+h);
    mztx06a_cmd_data(XP, x);
	mztx06a_cmd_data(YP, y);       
    mztx06a_cmd(0x202);    
    
    mztx06a_data(data, w * h * 2); 
    
    return 0;
}