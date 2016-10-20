#include <avr/io.h>
#include <avr/pgmspace.h>
#include "SSD1289x16.h"
#include "config.h"
//#include "uart.h"
#include <stdlib.h>
uint16_t _color = WHITE;
char buffer[12];

/********************************************************************************************************
* Function    : void LCD_WaitMs(unsigned int ms)
* Description : waits (pauses) for ms milliseconds (assumes internal clock at 8MHz)
* Input       : ms is wait milliseconds
* Returns     : none
* Notes       : none
*********************************************************************************************************/
void LCD_WaitMs(unsigned int ms)
{
    int i;

    while (ms-- > 0)
    {
        /* 8192 (8k) clock cycles for 1ms; each time through loop
           is 5 cycles (for loop control + nop) */
        for (i = 0; i < 1636; ++i)
            asm volatile ("nop");
    }
}
/********************************************************************************************************
* Function    : void LCD_WrCmd(unsigned char cmd)
* Description : This function performs low level command write to LCD
* Input       : cmd - is the command written to the LCD module
* Returns     : none
* Notes       : Hardware specific.
*********************************************************************************************************/
void LCD_WrCmd(uint8_t cmd)
{
    LCD_CLR_CS();   // clear CS pin to ENABLE LCD

    LCD_CLR_RS();
    LCD_SET_DBL(cmd);
    LCD_SET_DBH(cmd >> 8);
	LCD_CLR_WR();
	LCD_SET_WR();

    LCD_SET_CS();	// set CS pin to DISABLE LCD
}

/********************************************************************************************************
* Function    : void LCD_WrDat(unsigned int val)
* Description : This function performs low level display data write to LCD
* Arguments   : val - is the data written to the LCD module
* Returns     : none
* Notes       : Hardware specific.
********************************************************************************************************/
void LCD_WrDat(uint16_t val)
{
    LCD_CLR_CS();				// clear CS pin to ENABLE LCD
    LCD_SET_RS();
    LCD_SET_DBL(val);
    LCD_SET_DBH(val>>8);
	LCD_CLR_WR();
	LCD_SET_WR();

    LCD_SET_CS();	// set CS pin to DISABLE LCD
}

/*********************************************************************************************************
* Function:  void LCD_SetReg(unsigned char index, unsigned int val)
* Input       : index - register number
*             : val   - value to be set
* Returns     : none
* Notes       : Hardware specific.
**********************************************************************************************************/
void LCD_SetReg(uint8_t index, uint16_t val)
{
    LCD_WrCmd(index);
    LCD_WrDat(val);
}

/*********************************************************************
* Function    : void LCD_SetArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
* Description : set paint area
* Input       : x1,y1,x2,y2 - pixel coordinates
* Return      : none
* Note        : Side Effects: paint area change
********************************************************************/
void LCD_SetArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
#ifdef Horizontal
	LCD_SetReg(0x44,(x2 << 8) | x1);    // Source RAM address window
	LCD_SetReg(0x45,y1);    // Gate RAM address window
	LCD_SetReg(0x46,y2);    // Gate RAM address window
#else
	LCD_SetReg(0x44,(y2 << 8) | y1);    // Source RAM address window
	LCD_SetReg(0x45,x1);    // Gate RAM address window
	LCD_SetReg(0x46,x2);    // Gate RAM address window
#endif
    LCD_SetCursor(x1, y1);
}

/*********************************************************************************************************
* Function    : void LCD_InitPort(void)
* Description : This function init lcd io
* Input       : none
* Returns     : none
* Notes       : none
**********************************************************************************************************/
void LCD_InitPort(void)
{
    LCD_OUTPUT();
    // set default
    LCD_SET_CS();
    LCD_SET_RS();
    LCD_SET_WR();
    LCD_SET_RD();
    LCD_CLR_RST();

    // init direction
    LCD_DIR_CS(1);
    LCD_DIR_RS(1);
    LCD_DIR_WR(1);
    LCD_DIR_RD(1);
    LCD_DIR_RST(1);

    // reset module
    LCD_WaitMs(10);
    LCD_SET_RST();
    LCD_WaitMs(50);


}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */

/*********************************************************************************************************
* Function    : void LCD_Reset(void)
* Description : This function reset lcd module
* Input:      : none
* Returns     : none
* Notes       : none
**********************************************************************************************************/
void LCD_Reset(void)
{

	LCD_InitPort();

	LCD_WrCmd(0x0000);LCD_WrDat(0x0001); //Start Oscillation OSCEN=1
	LCD_WaitMs(15);
/*
	LCD_WrCmd(0x0003);LCD_WrDat(0xA2A4); //Power Control (1)
	LCD_WrCmd(0x000C);LCD_WrDat(0x0000); //Power Control (2)
	LCD_WrCmd(0x000D);LCD_WrDat(0x0308); //Power Control (3)
	LCD_WrCmd(0x000E);LCD_WrDat(0x3001); //Power Control (4)
	LCD_WrCmd(0x001E);LCD_WrDat(0x00AC); //Power Control (5)
*/

	LCD_WrCmd(0x0003);LCD_WrDat(0xAAAC); //Power Control (1)
	LCD_WrCmd(0x000C);LCD_WrDat(0x0002); //Power Control (2)
	LCD_WrCmd(0x000D);LCD_WrDat(0x000A); //Power Control (3)
	LCD_WrCmd(0x000E);LCD_WrDat(0x2C00); //Power Control (4)
	LCD_WrCmd(0x001E);LCD_WrDat(0x00B8); //Power Control (5)

	LCD_WaitMs(15);
	#ifndef Horizontal
		LCD_WrCmd(0x0001);LCD_WrDat(0x293F); //Driver Output Control RL=0, REV=1, BGR=1, TB=0
	#else
		LCD_WrCmd(0x0001);LCD_WrDat(0x2B3F); //Driver Output Control RL=0, REV=1, BGR=1, TB=1
	#endif
	LCD_WrCmd(0x0002);LCD_WrDat(0x0600);
	// Restore VSYNC mode from low power state
	LCD_WrCmd(0x0010);LCD_WrDat(0x0000); //Sleep mode cancel
	LCD_WrCmd(0x0011);LCD_WrDat(0x6030); //Entry Mode
										// DFM   0x4000 = 262L color
										// DFM   0x6000 = 65K color
										// AM    0x0000 = horizontal display
										// AM    0x0008 = Vertical display
										// ID[0] 0x0000 = horizontal decrement
										// ID[0] 0x0010 = horizontal increment
										// ID[1] 0x0000 = Vertical decrement
										// ID[1] 0x0020 = Vertical increment

	LCD_WaitMs(30);
	LCD_WrCmd(0x0005);LCD_WrDat(0x0000); // Compare register
	LCD_WrCmd(0x0006);LCD_WrDat(0x0000); // Compare register
	// Horizontal and Vertical porch are for DOTCLK mode operation
	LCD_WrCmd(0x0016);LCD_WrDat(0xEF1C); // Horizontal Porch
	LCD_WrCmd(0x0017);LCD_WrDat(0x0003); // Vertical Porch
	// Display Control
	LCD_WrCmd(0x0007);LCD_WrDat(0x0233); // Display Control
										 // D1 0x0000 = display off
										 // D1 0x0002 = display on
										 // D0 0x0000 = internal display halt
										 // D0 0x0001 = internal display operate

	LCD_WrCmd(0x000B);LCD_WrDat(0x5312); // Frame cycle control
	LCD_WrCmd(0x000F);LCD_WrDat(0x0000); // Gate Scan Position
	LCD_WaitMs(20);
	// Vertical Scroll Control
	LCD_WrCmd(0x0041);LCD_WrDat(0x0000); // Vertical Scroll Control
	LCD_WrCmd(0x0042);LCD_WrDat(0x0000); // Vertical Scroll Control

	// 1st Screen driving position
	LCD_WrCmd(0x0048);LCD_WrDat(0x0000); // Start position. 0
	LCD_WrCmd(0x0049);LCD_WrDat(0x013F); // End position.   319

	// Source RAM address
	LCD_WrCmd(0x0044);LCD_WrDat(0xEF00); //Horizontal RAM address position start/end setup
										 //dec 239
										 //dec 0, i.e. horizontal ranges from 0 -> 239
										 //POR value is 0xEF00 anyway. This address must be set before RAM write

	LCD_WrCmd(0x0045);LCD_WrDat(0x0000); //Vertical RAM address start position setting
										 //0x0000 = dec 0
	LCD_WrCmd(0x0046);LCD_WrDat(0x013F); //Vertical RAM address end position setting (0x013F = dec 319)

		// 2nd Screen driving position
//		LCD_WrCmd(0x004A);LCD_WrDat(0x0000); // Start position. 0
//		LCD_WrCmd(0x004B);LCD_WrDat(0x0000); // End position.   0
	LCD_WaitMs(20);
	//gamma control
	LCD_WrCmd(0x0030);LCD_WrDat(0x0707);
	LCD_WrCmd(0x0031);LCD_WrDat(0x0704);
	LCD_WrCmd(0x0032);LCD_WrDat(0x0204);
	LCD_WrCmd(0x0033);LCD_WrDat(0x0201);
	LCD_WrCmd(0x0034);LCD_WrDat(0x0203);
	LCD_WrCmd(0x0035);LCD_WrDat(0x0204);
	LCD_WrCmd(0x0036);LCD_WrDat(0x0204);
	LCD_WrCmd(0x0037);LCD_WrDat(0x0502);
	LCD_WrCmd(0x003A);LCD_WrDat(0x0302);
	LCD_WrCmd(0x003B);LCD_WrDat(0x0500);

	LCD_WaitMs(20);
}

/*********************************************************************
* Function    : void LCD_SetCursor(unsigned int x, unsigned int y)
* Description : set start address of lcd ram
* Input       : x,y - pixel coordinates
* Return      : none
* Note        : set start address change
********************************************************************/
void LCD_SetCursor(unsigned int x, unsigned int y)
{
#ifdef Horizontal
    LCD_SetReg(0x4E,x); // initial settings for the GDDRAM X address in the address counter (AC).
    LCD_SetReg(0x4F,y); // initial settings for the GDDRAM Y address in the address counter (AC).
#else
    LCD_SetReg(0x4E,y); // initial settings for the GDDRAM X address in the address counter (AC).
    LCD_SetReg(0x4F,x); // initial settings for the GDDRAM Y address in the address counter (AC).
#endif
    LCD_WrCmd(0x22);
}


/*********************************************************************
* Function    : void LCD_PutPixel(unsigned int x, unsigned int y)
* Description : puts pixel
* Input       : x,y - pixel coordinates
* Output      : none
* Note        : none
********************************************************************/
void LCD_PutPixel(unsigned int x, unsigned int y)
{
    LCD_SetCursor(x,y);
    LCD_WrDat(_color);
}

/*********************************************************************
* Function    : void LCD_Bar(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
* Description : draws rectangle filled with current color
* Input       : left,top - top left corner coordinates,
*             : right,bottom - bottom right corner coordinates
* Output      : none
* Note        :
*  CGRAM map
*  00000...000EF line0
*  00100...001EF line1
*  00200...002EF line2
*   ....
*  13F00...13FEF line320
*  yyyxx...yyyxx
*  address x = x
*  address y = y << 8
*  new line y = y + 0x100
********************************************************************/
void LCD_Bar(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int color)
{
    register unsigned int x,y;

    LCD_SetArea(left, top, right, bottom);
    for(y=top; y<=bottom; y++)
    {
        for(x=left; x<=right; x++)
        {
            LCD_FAST_WRITE(color);
        }
    }
    LCD_SetArea(0, 0, GetMaxX(), GetMaxY());
}

/*********************************************************************
* Function    : void LCD_Clear(unsigned int color)
* Description : fill display with color
* Input       : color - paint color
* Output      : none
* Note        : none
********************************************************************/
void LCD_Clear(unsigned int color)
{
//  LCD_WrCmd(0x0007);LCD_WrDat(0x0221); // Display Control: display off
    LCD_Bar(0, 0, GetMaxX(), GetMaxY(), color);
//  LCD_WrCmd(0x0007);LCD_WrDat(0x0233); // Display Control: display on
}

/*********************************************************************
* Function    : void DrawSymbol(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *pImage)
* Description : display image array to lcd
* Input       : x,y    - pixel coordinates
*             : w      - width
*             : h      - height
*             : pImage - FLASH array of image
* Output      : none
* Note        : none
********************************************************************/
void LCD_DrawSymbol(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const unsigned char *pImage)
{
    unsigned char colorMsb;
    unsigned char colorLsb;
    unsigned char colorDup = 0;
    unsigned int color=0;
    unsigned int x1,y1;

    // set draw area
    LCD_SetArea(x,y,x+w-1,y+h-1);

	for (x1 = 0; x1<w ; x1++)
	{
		for (y1 = 0; y1<h ; y1++)
		{
            if (!colorDup)
            {
                colorMsb = pgm_read_byte(pImage++);
                colorLsb = pgm_read_byte(pImage++);
                if (colorDup)
                    colorDup = pgm_read_byte(pImage++); // get duplicate no
                color = ((uint16_t)colorMsb << 8) | colorLsb;     // merge color
                SetColor(color);                        // set a pixel
            }
            else
            {
                colorDup--;
            }
            LCD_FAST_WRITE(color);
        }
    }
    LCD_SetArea(0, 0, GetMaxX(), GetMaxY());
}


