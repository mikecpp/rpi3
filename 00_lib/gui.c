#include <stdlib.h>
#include "gui.h"
#include "fonts.h"

#ifndef NULL
#define NULL 0x00
#endif

GUI_Interface_T *m_interface = NULL;
sFONT           *m_pFont     = NULL;
uint32_t         m_TextColor = 0x00FFFF00;
uint32_t         m_BackColor = 0x00000000;

#define ABS(X)  ((X) > 0 ? (X) : -(X))
#define TAG    "GUI"

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

//////////////////////////////////////////////////////////////////////////////
// Local Function

static int32_t DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
    uint32_t i = 0, j = 0;
    uint16_t height, width;
    uint8_t offset;
    uint8_t *pchar;
    uint32_t line;

    if(m_pFont == NULL) {
        return -1;
    }

    height = m_pFont->Height;
    width  = m_pFont->Width;
   
    offset =  8 *((width + 7)/8) -  width ;

    int32_t index = 0;
    uint8_t *data = malloc(width * height * 2);
    
    if(data == NULL) {
        return -1;
    }    
    
    for(i = 0; i < height; i++) {
        pchar = ((uint8_t *)c + (width + 7)/8 * i);

        switch(((width + 7)/8)) {
            case 1:
                line =  pchar[0];
                break;

            case 2:
                line =  (pchar[0]<< 8) | pchar[1];
                break;

            case 3:
            default:
                line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
                break;
        }
        
        for (j = 0; j < width; j++) {
            uint32_t s = width - j + offset - 1;
            if(line & (1 << s)) {
                *(data+index)   = RGB888to565(m_TextColor) >> 8;
                *(data+index+1) = RGB888to565(m_TextColor);
            }
            else {
                *(data+index)   = RGB888to565(m_BackColor) >> 8;
                *(data+index+1) = RGB888to565(m_BackColor);
            }
            index += 2;
         }
    }
         
    GUI_DrawRGBImage(Xpos, Ypos, width-1, height-1, (uint8_t*) data);

    if(data) {
        free(data);
    }    
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Public API

void GUI_Register(GUI_Interface_T *interface)
{
    m_interface = interface;
}

int32_t GUI_Init(void)
{
    if(m_interface == NULL) {
        return -1;
    }
    
    m_pFont = &Font24;

    return m_interface->GUI_Init();
}

int32_t GUI_GetXSize(void)
{
    if(m_interface == NULL) {
        return -1;
    }
    
    return m_interface->GUI_GetXSize();
}

int32_t GUI_GetYSize(void)
{
    if(m_interface == NULL) {
        return -1;
    }
    
    return m_interface->GUI_GetYSize();    
}



int32_t GUI_ReadPixel(uint16_t Xpos, uint16_t Ypos, uint32_t *Color)
{
    if(m_interface == NULL) {
        return -1;
    }
    
    return m_interface->GUI_ReadPixel(Xpos, Ypos, Color);
}

int32_t GUI_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t Color)
{
    if(m_interface == NULL) {
        return -1;
    }
    
    return m_interface->GUI_DrawPixel(Xpos, Ypos, Color);
}

int32_t GUI_Clear(uint32_t Color)
{
    if(m_interface == NULL) {
        return -1;
    }

    return m_interface->GUI_Clear(Color);
}

int32_t GUI_Update(void)
{
    if(m_interface == NULL || 
       m_interface->GUI_Update == NULL) {
        return -1;
    }        
    
    return m_interface->GUI_Update();
}

int32_t GUI_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pbmp)
{
    if(m_interface == NULL) {
        return -1;
    }

    return m_interface->GUI_DrawRGBImage(Xpos, Ypos, Xsize, Ysize, pbmp);
}

void GUI_SetFont(sFONT *fonts)
{
    m_pFont = fonts;
}

int32_t GUI_GetTextColor(uint32_t *Color)
{
    *Color = m_TextColor;

    return 0;
}

int32_t GUI_GetBackColor(uint32_t *Color)
{
    *Color = m_BackColor;

    return 0;
}

int32_t GUI_SetTextColor(uint32_t Color)
{
    m_TextColor = Color;

    return 0;
}

int32_t GUI_SetBackColor(uint32_t Color)
{
    m_BackColor = Color;

    return 0;
}

sFONT* GUI_GetFont(void)
{
    return m_pFont;
}

#define GUI_LINE(x) ((x) * (((sFONT *) GUI_GetFont())->Height))

void GUI_DisplayStringAtLine(uint16_t Line, uint8_t *ptr)
{
    GUI_DisplayStringAt(0, GUI_LINE(Line), ptr, GUI_LEFT_MODE); 
}

void GUI_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, GUI_Line_Mode_T Mode)
{
    uint16_t refcolumn = 1, i = 0;
    uint32_t size = 0, xsize = 0;
    uint8_t  *ptr = Text;

    while (*ptr++) {
        size ++ ;
    }

    xsize = (GUI_GetXSize() / m_pFont->Width);

    switch (Mode) {
        case GUI_CENTER_MODE:
            refcolumn = Xpos + ((xsize - size) * m_pFont->Width) / 2;
            break;
        case GUI_LEFT_MODE:
            refcolumn = Xpos;
            break;
        case GUI_RIGHT_MODE:
            refcolumn =  - Xpos + ((xsize - size) * m_pFont->Width);
            break;
        default:
            refcolumn = Xpos;
            break;
    }

    if ((refcolumn < 1) || (refcolumn >= 0x8000)) {
        refcolumn = 1;
    }

    while ((*Text != 0) & (((GUI_GetXSize() - (i*m_pFont->Width)) & 0xFFFF)
           >= m_pFont->Width))
    {
        GUI_DisplayChar(refcolumn, Ypos, *Text);
        refcolumn += m_pFont->Width;
        Text++;
        i++;
    }
}

void GUI_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    uint32_t index = (Ascii - 0x20) * m_pFont->Height * ((m_pFont->Width + 7) / 8);

    DrawChar(Xpos, Ypos, &(m_pFont->table[index]));
}

void GUI_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    uint32_t index = 0;

    for(index = 0; index < Length; index++) {
       GUI_DrawPixel((Xpos + index), Ypos, m_TextColor);
    }
}

void GUI_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    uint32_t index = 0;

    for(index = 0; index < Length; index++) {
        GUI_DrawPixel(Xpos, Ypos + index, m_TextColor);
    }
}

void GUI_DrawLine(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    curpixel = 0;

    deltax = ABS(X2 - X1);
    deltay = ABS(Y2 - Y1);
    x = X1;
    y = Y1;

    if (X2 >= X1) {
        xinc1 = 1;
        xinc2 = 1;
    }
    else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (Y2 >= Y1) {
        yinc1 = 1;
        yinc2 = 1;
    }
    else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd    = deltay;
        numpixels = deltax;
    }
    else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd    = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        GUI_DrawPixel(x, y, m_TextColor);
        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void GUI_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    GUI_DrawHLine(Xpos, Ypos, Width);
    GUI_DrawHLine(Xpos, (Ypos+ Height), Width);

    GUI_DrawVLine(Xpos, Ypos, Height);
    GUI_DrawVLine((Xpos + Width), Ypos, Height);
}

void GUI_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t   decision;
    uint32_t  current_x;
    uint32_t  current_y;

    decision = 3 - (Radius << 1);
    current_x = 0;
    current_y = Radius;

    while (current_x <= current_y) {
        GUI_DrawPixel((Xpos + current_x), (Ypos - current_y), m_TextColor);
        GUI_DrawPixel((Xpos - current_x), (Ypos - current_y), m_TextColor);
        GUI_DrawPixel((Xpos + current_y), (Ypos - current_x), m_TextColor);
        GUI_DrawPixel((Xpos - current_y), (Ypos - current_x), m_TextColor);
        GUI_DrawPixel((Xpos + current_x), (Ypos + current_y), m_TextColor);
        GUI_DrawPixel((Xpos - current_x), (Ypos + current_y), m_TextColor);
        GUI_DrawPixel((Xpos + current_y), (Ypos + current_x), m_TextColor);
        GUI_DrawPixel((Xpos - current_y), (Ypos + current_x), m_TextColor);
        if (decision < 0) {
            decision += (current_x << 2) + 6;
        }
        else {
            decision += ((current_x - current_y) << 2) + 10;
            current_y--;
        }
        current_x++;
    }
}

void GUI_DrawEllipse(uint16_t Xpos, uint16_t Ypos, uint16_t XRadius, uint16_t YRadius)
{
    int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
    float k = 0, rad1 = 0, rad2 = 0;

    rad1 = XRadius;
    rad2 = YRadius;

    k = (float)(rad2/rad1);

    do {
        GUI_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), m_TextColor);
        GUI_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), m_TextColor);
        GUI_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), m_TextColor);
        GUI_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), m_TextColor);

        e2 = err;
        if (e2 <= x) {
            err += ++x*2+1;
            if (-y == x && e2 <= y)
                e2 = 0;
        }
        if (e2 > y)
            err += ++y*2+1;
    } while (y <= 0);
}

void GUI_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    do {
        GUI_DrawHLine(Xpos, Ypos++, Width);
    }
    while(Height--);
}

void GUI_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t   decision;
    uint32_t  current_x;
    uint32_t  current_y;

    decision = 3 - (Radius << 1);

    current_x = 0;
    current_y = Radius;

    while (current_x <= current_y)
    {
        if(current_y > 0) {
            GUI_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
            GUI_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
        }

        if(current_x > 0) {
            GUI_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
            GUI_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
        }

        if (decision < 0) {
            decision += (current_x << 2) + 6;
        }
        else {
            decision += ((current_x - current_y) << 2) + 10;
            current_y--;
        }
        current_x++;
    }

    GUI_DrawCircle(Xpos, Ypos, Radius);
}

void GUI_FillEllipse(uint16_t Xpos, uint16_t Ypos, uint16_t XRadius, uint16_t YRadius)
{
    int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
    float k = 0, rad1 = 0, rad2 = 0;

    rad1 = XRadius;
    rad2 = YRadius;

    k = (float)(rad2/rad1);

    do {
        GUI_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
        GUI_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));

        e2 = err;
        if (e2 <= x) {
            err += ++x*2+1;
            if (-y == x && e2 <= y)
                e2 = 0;
        }
        if (e2 > y)
            err += ++y*2+1;
    }

    while (y <= 0);
}
