#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "max7219.h"

extern uint8_t Matrix_Font8[];
    
static char m_data[8];
SPI_ID m_id = SPI_1; 

void max7219_redraw(void)
{
    int i;
    
    for(i=1; i<9; i++) {
        max7219_write(i, m_data[i-1]);
    }    
}

char reverse_byte(char b) 
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    
    return b;
}

//////////////////////////////////////////////////////////////////////////////
// Public API
//
int max7219_init(SPI_ID id)
{
    m_id = id;
    
    spi_init(m_id);
    
    max7219_write(0x09, 0x00);
    max7219_write(0x0A, 0x01);
    max7219_write(0x0B, 0x07);
    max7219_write(0x0C, 0x01);
    max7219_write(0x0F, 0x00);
    
    int i;
    for(i=0; i<8; i++) {
        m_data[i] = 0x00;
    }  
    
    return 0;
}

void max7219_write(uint8_t addr, uint8_t data)
{
    uint8_t buf[2];
    
    buf[0] = addr;
    buf[1] = data;
    
    spi_write(m_id, buf, 2);
}

void max7219_intensity(uint8_t value)
{
    if(value >=0x0F) {
        value = 0x0F;
    }    
    max7219_write(0x0A, value);
}

void max7219_test(uint8_t enable)
{
    if(enable == 0x00) {
        max7219_write(0x0F, 0x00);
    }
    else {
        max7219_write(0x0F, 0x00);
    }
}

void max7219_pixel(uint8_t x, uint8_t y, uint8_t enable)
{
    if(x >= 8) {
        x = 8;
    }
    
    if(y >= 8) {
        y = 8;
    }
    
    if(enable == 0) {
        m_data[y-1] &= ~(1 << (8-x));
    }
    else {
        m_data[y-1] |=  (1 << (8-x));
    }    
    
    max7219_redraw();
}

void max7219_char(char ch)
{
    char    index = ch - 32;
    uint8_t *pch  = &Matrix_Font8[index*8];
    
    int i;

    for(i=0; i<8; i++){
        max7219_write(i+1, reverse_byte(*(pch+i)) >> 1);
    }
}

void max7219_scroll_char(char ch)
{
    char    index = ch - 32;
    uint8_t *pch  = &Matrix_Font8[index*8];
    
    int i, j;

    for(i=0; i<8; i++) {
        m_data[i] = reverse_byte(*(pch+i)); 
    }    
    
    for(j=0; j<8; j++) {
        for(i=0; i<8; i++){
            max7219_write(i+1, m_data[i] << j);
        }
        delay_ms(250);
    }    
}

void max7219_scroll_string(char *str)
{
    char *ptr = str;
    
    while(*ptr) {
        max7219_scroll_char(*(ptr++));
    }
}
