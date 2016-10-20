/*
 * config.h
 *
 * Created: 2/19/2013 10:41:04 AM
 */ 
#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <avr/io.h>
#include "config.h"
/* _____HARDWARE DEFINES_____________________________________________________ */
//rs pa0
//wr pa1
//rd pa2
//rest pa3
//cs pa4

//0-7 LO PD
//8-16 HI PB
#define LCD_LO_DDR  DDRA
#define LCD_LO_PORT PORTA
#define LCD_LO_PIN  PINA

#define LCD_HI_DDR  DDRC
#define LCD_HI_PORT PORTC
#define LCD_HI_PIN  PINC

#define LCD_RS_DDR  DDRE
#define LCD_RS_PORT PORTE
#define LCD_RS_PIN  PINE
#define LCD_RS_BIT  4

#define LCD_CS_DDR  DDRE
#define LCD_CS_PORT PORTE
#define LCD_CS_PIN  PINE
#define LCD_CS_BIT  5

#define LCD_RD_DDR  DDRE
#define LCD_RD_PORT PORTE
#define LCD_RD_PIN  PINE
#define LCD_RD_BIT  6

#define LCD_WR_DDR  DDRE
#define LCD_WR_PORT PORTE
#define LCD_WR_PIN  PINE
#define LCD_WR_BIT  7

#define LCD_RST_DDR  DDRE
#define LCD_RST_PORT PORTE
#define LCD_RST_PIN  PINE
#define LCD_RST_BIT  2

#define LCD_SET_CS()  LCD_CS_PORT  |=   1<<LCD_CS_BIT
#define LCD_CLR_CS()  LCD_CS_PORT  &= ~(1<<LCD_CS_BIT)
#define LCD_DIR_CS(x) LCD_CS_DDR   |=   1<<LCD_CS_BIT

#define LCD_SET_RS()  LCD_RS_PORT  |=   1<<LCD_RS_BIT
#define LCD_CLR_RS()  LCD_RS_PORT  &= ~(1<<LCD_RS_BIT)
#define LCD_DIR_RS(x) LCD_RS_DDR   |=   1<<LCD_RS_BIT

#define LCD_SET_WR()  LCD_WR_PORT  |=   1<<LCD_WR_BIT
#define LCD_CLR_WR()  LCD_WR_PORT  &= ~(1<<LCD_WR_BIT)
#define LCD_DIR_WR(x) LCD_WR_DDR   |=   1<<LCD_WR_BIT

#define LCD_SET_RD()  LCD_RD_PORT  |=   1<<LCD_RD_BIT
#define LCD_CLR_RD()  LCD_RD_PORT  &= ~(1<<LCD_RD_BIT)
#define LCD_DIR_RD(x) LCD_RD_DDR   |=   1<<LCD_RD_BIT

#define LCD_SET_RST()  LCD_RST_PORT |=  1<<LCD_RST_BIT
#define LCD_CLR_RST()  LCD_RST_PORT &= ~(1<<LCD_RST_BIT)
#define LCD_DIR_RST(x) LCD_RST_DDR  |=  1<<LCD_RST_BIT

#define LCD_INPUT()		{LCD_LO_DDR = 0x00; LCD_HI_DDR = 0x00;}
#define LCD_OUTPUT()	{LCD_LO_DDR = 0xFF; LCD_HI_DDR = 0xFF;}

#define LCD_MODE()

#define LCD_SET_DBH(x)	LCD_HI_PORT = x
#define LCD_SET_DBL(x)	LCD_LO_PORT = x

#define LCD_GET_DBH()	LCD_HI_PIN
#define LCD_GET_DBL()	LCD_LO_PIN
/* _____PUBLIC DEFINE_____________________________________________________ */
//#define Horizontal

#ifdef Horizontal
// Horizontal and vertical screen size
#define SCREEN_HOR_SIZE    240UL
#define SCREEN_VER_SIZE    320UL

#else
// Horizontal and vertical screen size
#define SCREEN_HOR_SIZE    320UL
#define SCREEN_VER_SIZE    240UL

#endif

/* No touch zone follows */	
#define DPLIO _SFR_IO_ADDR(DATA_PORT_LOW)
#define DPHIO _SFR_IO_ADDR(DATA_PORT_HIGH)

#endif /* __CONFIG_H__ */
