/**************************************************************************************************
* File name		: graphic.h
* Programmer 	: jaruwit supa
* Web presence  : www.thaibestlink.com
* Note			: lcd graphic support
* Language		: avrGCC
* Hardware		: atmega16
* Date			: 01/05/2009
***************************************************************************************************/

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

/* _____PROJECT INCLUDES_____________________________________________________ */
// LCM driver define
#include "SSD1289x16.h"


/* _____PUBLIC FUNCTIONS_____________________________________________________ */
extern void Line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern void Rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern void RoundRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int radius, unsigned char fill);
extern void Circle(unsigned int x, unsigned int y, unsigned int radius, unsigned char fill);

/* _____PUBLIC DEFINE________________________________________________________ */
#define PutPixel(x,y)				LCD_PutPixel(x,y)
#define FillRectangle(x1,y1,x2,y2)	LCD_Bar(x1, y1, x2, y2, GetColor())

#define Bevel(x1,y1,x2,y2,r) 		RoundRectangle(x1,y1,x2,y2,r,0)
#define BevelFill(x1,y1,x2,y2,r)	RoundRectangle(x1,y1,x2,y2,r,1)


#endif
