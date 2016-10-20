#ifndef __SSD1289X16_H__
#define __SSD1289X16_H__

#include "config.h"

// color
#define BLACK                       RGB(0x00, 0x00, 0x00)
#define WHITE                       RGB(0xFF, 0xFF, 0xFF)
#define RED                         RGB(0xFF, 0x00, 0x00)
#define GREEN                       RGB(0x00, 0xFF, 0x00)
#define BLUE                        RGB(0x00, 0x00, 0xFF)
#define YELLOW                      RGB(0xFF, 0xFF, 0x00)
#define MAGENTA                     RGB(0xFF, 0x00, 0xFF)
#define CYAN                        RGB(0x00, 0xFF, 0xFF)
#define GRAY                        RGB(0x80, 0x80, 0x40)
#define SILVER                      RGB(0xA0, 0xA0, 0x80)
#define GOLD                        RGB(0xA0, 0xA0, 0x40)

/* _____PUBLIC VARIABLE_____________________________________________________ */
extern unsigned int _color;

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
extern void LCD_Reset(void);
extern void LCD_SetCursor(unsigned int x, unsigned int y);
extern void LCD_SetArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern void LCD_PutPixel(unsigned int x, unsigned int y);
extern void LCD_Bar(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int color);
extern void LCD_Clear(unsigned int color);
extern void LCD_DrawSymbol(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const unsigned char *pImage);
extern void LCD_WrDat(uint16_t dat);
/* _____DEFINE MACRO_________________________________________________________ */
// fast write is macro define. prevent call routine
#define LCD_FAST_WRITE(val)		LCD_CLR_CS(); \
								LCD_SET_RS(); \
								LCD_SET_DBH((val)>>8); \
								LCD_SET_DBL(val); \
								LCD_CLR_WR(); \
								LCD_SET_WR(); \
							    LCD_SET_CS()

#define LCD_FAST_WRITE2(H,L)	LCD_CLR_CS(); \
								LCD_SET_RS(); \
								LCD_SET_DBH(H); \
								LCD_SET_DBL(L); \
								LCD_CLR_WR(); \
								LCD_SET_WR(); \
							    LCD_SET_CS()

#define GetMaxX() 		((unsigned int)SCREEN_HOR_SIZE-1)
#define GetMaxY() 		((unsigned int)SCREEN_VER_SIZE-1)

#define RGB(red, green, blue)	((unsigned int)( (( red >> 3 ) << 11 ) | \
								(( green >> 2 ) << 5  ) | \
								( blue  >> 3 )))

#define SetColor(color) _color = color
#define GetColor()      _color

#endif
