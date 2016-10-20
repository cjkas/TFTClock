/***********************************************************************************************
*	  			  display bmp 24 bit with circuitidea.com BL-TFT240320PLUS
* File name		: sdbmp.h
* Programmer 	: jaruwit supa
* Web presence  : www.circuitidea.com
* Note			: display bmp on qvga display
* Language		: avrGCC
* Hardware		: atmega16
* Date			: 19/09/2009
*************************************************************************************************/

#ifndef __SDBMP_H__
#define __SDBMP_H__

extern unsigned long bmpWidth;           //Width (pixels)
extern unsigned long bmpHeight;          //Height (pixels)


extern FRESULT bmpFile(char * pFile);
extern void bmpShow(unsigned int x, unsigned int y);

#endif
