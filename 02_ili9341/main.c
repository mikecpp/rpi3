#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bcm2835.h"
#include "rpi_spi.h" 
#include "spi.h"
#include "ili9341.h"
#include "gui.h"

#define WIDTH   17
#define HEIGHT  24
    
uint8_t data[WIDTH * HEIGHT * 2] = {0}; 

uint32_t color[] = {GUI_COLOR_BLACK, GUI_COLOR_NAVY, GUI_COLOR_DARKGREEN, GUI_COLOR_DARKCYAN, GUI_COLOR_MAROON,
                       GUI_COLOR_PURPLE, GUI_COLOR_OLIVE, GUI_COLOR_LIGHTGREY, GUI_COLOR_DARKGREY, GUI_COLOR_BLUE,
                       GUI_COLOR_GREEN, GUI_COLOR_CYAN, GUI_COLOR_RED, GUI_COLOR_MAGENTA, GUI_COLOR_YELLOW,
                       GUI_COLOR_WHITE, GUI_COLOR_ORANGE, GUI_COLOR_GREENYELLOW, GUI_COLOR_PINK, GUI_COLOR_BLACK};

void main()
{   
    int count = 0;
    char msg[32];
    
    // Register HAL interface
    spi_register_driver(&rpi_spi_drv);
    GUI_Register(&ili9341_gui_interface);
    
    GUI_Init();

    int i = 0;
    for(i=0; i<20; i++) {
        GUI_Clear(color[i]);
        delay(100);
    }
    
    GUI_Clear(GUI_COLOR_BLACK);     
    GUI_SetBackColor(GUI_COLOR_BLACK);
    GUI_SetTextColor(GUI_COLOR_YELLOW);
    GUI_DisplayStringAt(0, 0, (uint8_t*) "Hello", GUI_CENTER_MODE); 
    GUI_DisplayStringAtLine(3, (uint8_t*) "Hello Mike");

    while (1)
    {
        sprintf(msg, "Count = %d", count++); 
        GUI_DisplayStringAtLine(5, (uint8_t*) msg);
        delay(100);
    }

}
