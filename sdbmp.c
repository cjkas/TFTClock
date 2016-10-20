/***********************************************************************************************
*	  			  display bmp 24 bit with circuitidea.com BL-TFT240320PLUS
* File name		: sdbmp.h
* Programmer 	: jaruwit supa
* Web           : www.circuitidea.com
* Note			: display bmp on qvga display
* Language		: avrGCC
* Hardware		: atmega16
* Date			: 19/09/2009
*************************************************************************************************
* DESCRIPTION
* ===========
* Reading BMP from SD card
* using : Petit FatFs - FAT file system module  R0.01a (C)ChaN, 2009
*		  
* Display on TFT QVGA circuitidea.com BL-TFT240320PLUS
* support only bmp 24 bit
* support BMP padding data. Option for complex BMP (image width devide by 4 giving 0).
* for small size and fast display.
*
* BMP padding
* -----------
* in BMP total horizontal bytes must devide by 4. remain is padding 0-3 byte.
* For fast way to display picture. do this.
* 1. create picture width can device by 4 give 0
* 2. using FILL IN AREA mode of QVGA.
*
* HISTORY
* -------
* 01/10/2009 Using stream mode for faster and smooter.
* 	         **NOTE** 
*            Becare full to set stream parameter before use function. bmpShow has no link to FATFS (FATFS not public variable).
*
*			 fs.flag |= FA_STREAM;	// Set stream mode
*			 bmpShow((unsigned int)SCREEN_HOR_SIZE/2 - w/2, (unsigned int)SCREEN_VER_SIZE/2 - h/2);
*			 fs.flag &= ~FA_STREAM;	// Clear stream mode
* 
************************************************************************************************/

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
/* _____PROJECT INCLUDES_____________________________________________________ */
#include "diskio.h"
#include "pff.h"
#include "graphic.h"

/* _____LOCAL DEFINE_________________________________________________________ */
typedef struct BMIH { //declares BitMap Info Header structure
	unsigned long biSize;            //Length of bitmap information header (40 bytes for Windows V3 bitmaps)
	unsigned long biWidth;           //Width (pixels)
	unsigned long biHeight;          //Height (pixels)
	unsigned short biPlanes;         //Color planes, always 1
	unsigned short biBitCount;       //Color bits per pixel (1, 4, 8, 16, 24 and 32)
	unsigned long biCompression;     //Compression method, we only read uncompressed (type 0)
	unsigned long biSizeImage;       //Image data length
	unsigned long biXPelsPerMeter;   //Horizontal resolution (pixels per meter)
	unsigned long biYPelsPerMeter;   //Vertical resolution (pixel per meter)
	unsigned long biClrUsed;         //Number of colors, ignored.
	unsigned long biClrImportant;    //Number of important colors, ignored.                                  
}BMP_INFO;

typedef struct BMFH { //declares BitMap File Header structure
	unsigned short bfType;           //Always 0x42 0x4D (hex for BM <-- indicating a bitmap)
	unsigned long  bfSize;           //File size (bytes)
	unsigned short bfReserved1;      //Reserved, ignored
	unsigned short bfReserved2;      //Reserved, ignored
	unsigned long  bfOffBits;        //Location in file of the first bitmap data               
}BMP_HEADER;


/* _____LOCAL VARIABLES______________________________________________________ */
unsigned long bmpWidth;           //Width (pixels)
unsigned long bmpHeight;          //Height (pixels)
char buffer[10];

/* _____PRIVATE FUNCTIONS____________________________________________________ */

/* _____PUBLICE FUNCTIONS____________________________________________________ */
FRESULT bmpFile(char * pFile)
{
	BMP_HEADER BMPHeader;
	BMP_INFO BMPInfo;

	FRESULT res;
    WORD br;			// File Read count

	res = pf_open(pFile);
	if (res)
    	return res;

	res = pf_read(&BMPHeader, sizeof(BMPHeader), &br);	// read bmp header
	if (res)
    	return res;

	if (BMPHeader.bfType != 0x4d42 )		// type is 'BM'
    	return BMP_BAD_HEAD;

	res = pf_read(&BMPInfo, sizeof(BMPInfo), &br);	// read bmp info
	if (res)
    	return res;

	if (BMPInfo.biSize        !=  40) return BMP_UNK_HEAD; //unknown header format/length
	if (BMPInfo.biPlanes      !=   1) return BMP_BIPLANES; //this should be 1
	if (BMPInfo.biWidth       >  320) return BMP_OVR_WIDTH; //image over size
	if (BMPInfo.biHeight      >  320) return BMP_OVR_HEIGHT; //image over size
	if (BMPInfo.biCompression !=   0) return BMP_COMPRESSED; //0=no compression

	// set data
	bmpHeight  = BMPInfo.biWidth;
	bmpWidth = BMPInfo.biHeight;

	// move to start of bmp data
	res = pf_lseek(BMPHeader.bfOffBits);
	if (res)
    	return res;
    
	return FR_OK;
}
/*
// support BMP 24 bit True color only
void bmpShow(unsigned int x, unsigned int y)
{
	FRESULT res;
    WORD br;			// File Read count
    BYTE buffer[512];	// file copy buffer
	unsigned int i;		// buffer counter

	BYTE cc;			// color counter
    BYTE B,G,R;			// color B,G,R

    UINT HCount = 0;	// Horizontal counter
	BYTE padding;		// pad size
    BYTE p;				// pad counter

// *** 24 bit True color Image ***
    padding = (4 - ((bmpWidth*3) % 4))%4;
    cc = 0;						// start with B-R-G

	// set draw area
	LCD_SetArea(x, y, x+bmpWidth-1, y+bmpHeight-1);
    do
    {
        res = pf_read(buffer, sizeof(buffer), &br);  // read data
		if (res)
   			break;

        for (i=0; i< br; i++)
        {
            // put color
            switch (cc)
            {
                case 0: B = buffer[i] >> 3; cc++; break;
                case 1: G = buffer[i] >> 2; cc++; break;
                case 2: R = buffer[i] >> 3; cc=0;
						LCD_FAST_WRITE(((R << 11) | (G << 5) | B));
            			HCount++;
						if (HCount >= bmpWidth)			// not same line
						{
							HCount = 0;					// reset Horizontal count
							if (padding)				// padding data
							{
							    p = padding;			// assign padding
								cc = 3;					// start padding mode
							}
						}
                        break;

				default:								// padding
						p--;
						if (!p)							// end of padding
							cc=0;						//   start B-G-R
                        break;
            }
   		}
    } while (res || br == sizeof(buffer));  	// error or eof 

	// restore area
	LCD_SetArea(0, 0, GetMaxX(), GetMaxY());
}
*/

BYTE cc;			// color counter
BYTE CB,CG,CR;		// color B,G,R
UINT HCount;		// Horizontal counter
BYTE padding;		// padding size
BYTE pad;			// pad

BYTE bmpDisp(BYTE c)
{
//	UINT pixel;
	switch (cc)
    {
    	case 0: CB = c >> 3; cc++; break;
    	case 1: CG = c >> 2; cc++; break;
		case 2: CR = c >> 3; cc=0;
//			uint16_t pixel = ((CR << 11) | (CG << 5) | CB );
//			LCD_WrDat(pixel);
			LCD_FAST_WRITE2((CR << 3) | (CG >> 3), (CG << 5) | CB );

			if (padding)					// padding data
			{
				itoa(HCount,buffer,10);

				HCount++;
				if (HCount >= bmpWidth)		// not same line
				{
					HCount = 0;				// reset Horizontal count
				    pad = padding;			// assign padding
					cc = 3;					// start padding mode
				}
			}
            break;

		default:							// padding
			pad--;
			if (!pad)						// end of padding
				cc=0;						//   start B-G-R
            break;
	}
    return 1;       // Return with success
                    // When return with zero, pf_read fails with FR_STREAM_ERR;
}
// support BMP 24 bit True color only
void bmpShow(unsigned int x, unsigned int y)
{
	FRESULT res;
    WORD br;			// File Read count

// *** 24 bit True color Image ***
    padding = (4 - ((bmpWidth*3) % 4))%4;
	HCount = 0;			// set Horizontal counter
    cc = 0;				// start with B-R-G

	// set draw area
	LCD_SetArea(x, y, x+bmpWidth-1, y+bmpHeight-1);

    // Forward data to the stream
    do
    {
        res = pf_read(bmpDisp, 0x7FFF, &br);		// read data
    } while (res || (br == 0x7FFF));  	// error or eof 

	// restore area
	LCD_SetArea(0, 0, GetMaxX(), GetMaxY());
}
