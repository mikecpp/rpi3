#include <string.h>

#include "oled1602.h" 
#include "util.h"

// oled1602_commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for function set
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

// flags for backlight control
#define LCD_BACKLIGHT           0x08
#define LCD_NOBACKLIGHT         0x00

// flags for function set 
#define LCD_2OR4LINE            0x08  
#define LCD_1OR3LINE            0x00  
#define LCD_EXTENSIONREGISTERON 0x02  
#define LCD_DOUBLEHEIGHTON      0x04  
#define LCD_DOUBLEHEIGHTOFF     0x00  

// flags for extended function set  
#define LCD_6DOTS               0x04
#define LCD_5DOTS               0x00
#define LCD_INVERTCURSORON      0x02
#define LCD_3OR4LINE            0x01 
#define LCD_1OR2LINE            0x00 

#define msleep(val)             delay_ms(val)
#define OLED1602_ADDR           0x3C

void home(void);
int oled1602_command(uint8_t value);
int oled1602_data(uint8_t value);

static uint8_t _row_offsets[4] = {0x00, 0x40, 0x40, 0x60};
static uint8_t _displayfunction;           // RE = 0
static uint8_t _displaycontrol;
static uint8_t _numlines = 2;

static uint8_t m_i2c_id = 0xFF;

int oled1602_command(uint8_t value)
{
    int ret = 0;
    uint8_t buf[2];
    
    buf[0] = 0x00;
    buf[1] = value;
    ret = i2c_write(m_i2c_id, OLED1602_ADDR, buf, 2); 
    
    return ret;
}

int oled1602_data(uint8_t value)
{   
    int ret = 0;
    uint8_t buf[2];
    
    buf[0] = 0x40;
    buf[1] = value;
    ret = i2c_write(m_i2c_id, OLED1602_ADDR, buf, 2);
    
    return ret;
}

void home(void)
{
    oled1602_command(LCD_RETURNHOME);
    msleep(2);     
}

void setRE(void)
{
    _displayfunction |= LCD_EXTENSIONREGISTERON;
    oled1602_command(LCD_FUNCTIONSET | _displayfunction);    
}

void clearRE(void)
{
    _displayfunction &= ~LCD_EXTENSIONREGISTERON;
	oled1602_command(LCD_FUNCTIONSET | _displayfunction);   
}

void set_contrast(uint8_t value)
{
	setRE();

	oled1602_command(0x79);          // set SD to 1; enter OLED oled1602_command set
	oled1602_command(0x81);  	    // Set Contrast
	oled1602_command(value);	        // send contrast value
	
    oled1602_command(0x78);  	    // clear SD; Exiting Set OLED oled1602_command set
  
    clearRE();    
}

void fade_out(uint8_t value)
{
    setRE();
	
    oled1602_command(0x79);          // set SD to 1; enter OLED oled1602_command se
	oled1602_command(0x23);  	    // Set 
	oled1602_command(0x30 | value);	// send 
    oled1602_command(0x78);  	    // clear SD; Exiting Set OLED oled1602_command set

    clearRE();
}

int oled1602_init(uint8_t i2c_id)
{
    m_i2c_id = i2c_id;
    
    i2c_init(m_i2c_id);
    
    msleep(100);
    
    // Enable internal Regulator
    oled1602_command(0x2A);
    oled1602_command(0x71);
    oled1602_data(0x5C);
    oled1602_command(0x28);
    
    // Set Display OFF
    oled1602_command(0x08);
    
    // Set Display Clock Divide Rate 
    oled1602_command(0x2A);
    oled1602_command(0x79);
    oled1602_command(0xD5);
    oled1602_command(0x70);
    oled1602_command(0x78);
    
    // Set 1~2 lines
    oled1602_command(0x08);
    
    // Set Re-MAP mode
    oled1602_command(0x06);
    
    // CGRAM
    oled1602_command(0x72);
    oled1602_data(0x05);
    
    // Set OLED Characterization
    oled1602_command(0x79);
    
    // Set SEG Pins
    oled1602_command(0xDA);
    oled1602_command(0x10);
    
    oled1602_command(0xDC);
    oled1602_command(0x03);
    
    msleep(100);
    
    // Contrast
    oled1602_command(0x81);
    oled1602_command(0xFF);
    
    // Set Pre-Charge Pwriod
    oled1602_command(0xD9);
    oled1602_command(0xF1);
    
    // Set VCOMH dis-select level
    oled1602_command(0xDB);
    oled1602_command(0x40);
    
    // Exit OLED Characterization
    oled1602_command(0x78);
    oled1602_command(0x28);
    
    // Set Display OFF
    oled1602_command(0x08);
    
    // oled1602_command(0x40);
    // i2c_write(m_i2c_id, (uint8_t*) CGRAM1, 40);
    
    // Clear Display
    oled1602_command(0x01);
    msleep(500);
    
    // Set DDRAM Address
    oled1602_command(0x80);
    msleep(100);
    
    // Turn ON Display
    oled1602_command(0x0C);
    
 	_displaycontrol         = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    _displayfunction        = LCD_2OR4LINE  | LCD_DOUBLEHEIGHTOFF;        
       
    set_contrast(0x10);
       
    return 0;
}

int oled1602_puts(char *s)
{
    uint32_t i;
    
    if(s == 0x00)
        return -1;
    
    for(i = 0; i<strlen((char*) s); i++) {
        oled1602_data(s[i]);
    }
    
    return 0;
}

int oled1602_clear(void)
{
    oled1602_command(LCD_CLEARDISPLAY);
    msleep(2);
    
    return 0;
}

int oled1602_cursor(uint8_t col, uint8_t row)
{
    if ( row > _numlines ) {
		row = _numlines - 1;    
	}  
    
    oled1602_command(0x80 | (col + _row_offsets[row]));
    
    return 0;
}

int oled1602_backlight(bool enable)
{  
    return 0;
}

void oled1602_on(void)
{
	_displaycontrol |= LCD_DISPLAYON;
	oled1602_command(LCD_DISPLAYCONTROL | _displaycontrol);    
}

void oled1602_off(void)
{
	_displaycontrol &= ~LCD_DISPLAYON;
	oled1602_command(LCD_DISPLAYCONTROL | _displaycontrol);    
}

void oled1602_display(void)
{
    oled1602_on();
}
