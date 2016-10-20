
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "i2cmaster.h"
#include "pcf_helper.h"
#include "ds18b20.h"

#include "SSD1289x16.h"
#include "graphic.h"
#include "font.h"
#include "fn_big.h"
#include "ft_med.h"
#include "diskio.h"
#include "pff.h"
#include "sdbmp.h"
#include "img_temp.h"
#include "img_btm.h"
#include "img_min.h"
#include "img_hour.h"

#define DEBUG 1

#ifdef DEBUG
#include "uart.c"
#define UART_BAUD_RATE      38400
#endif



#ifndef irPin
#define irPin (PINE & (1<<PINE2))
#endif

volatile char int2_=0;
char buffer[50];
char fname[100];
unsigned int signal[200];
unsigned int bignal[200];
unsigned int res[10];
unsigned int i = 0;
unsigned int s = 0;
unsigned int h = 0;

void getIR(void) {
	s = 0;
	h = 0;
	unsigned int lTime;
	unsigned int FTime;

	//start bit 3500
	if (!irPin)		 //wait for it to be low

	lTime = 0; //reset the counter

	while (irPin == 0) { //while the pin is low which is our pulse count
		lTime++; //increment every 200uS until pin is high
		_delay_us(99); //200uS delay
		if(lTime>=37)
			return;
	}

	if (lTime <= 34) //Start too short
		return; //Restart
	if (lTime >= 37) //Start too long
		return; //Restart

	//space 1700
	while (irPin == 0)
		; //wait for it to be low
	lTime = 0; //reset the counter

	while (irPin) { //while the pin is low which is our pulse count
		lTime++; //increment every 200uS until pin is high
		_delay_us(100); //200uS delay
		if(lTime>17)
			return;
	}

	if (lTime <= 15) //Start too short
		return; //Restart
	if (lTime >= 17) //Start too long
		return;

	lTime = 0;

	while (1) {
		FTime = 0;
		while (irPin == 0) {
			FTime++;
			_delay_us(9);
			if (FTime > 5000) {
				return;
			}
		}
		if (FTime > 1) {
			signal[s] = FTime;
			s++;
		}

		lTime = 0; //reset the counter

		while (irPin) //wait for it to be low
		{
			lTime++;
			_delay_us(9);
			if (lTime > 5000) {
				return;
			}
		}
		if (lTime > 1) {
			signal[s] = lTime;
			s++;
		}
		h++;
		if (FTime < 100 && lTime > 100) {
			bignal[h] = 1;
		} else if (FTime < 100 && lTime < 100) {
			bignal[h] = 0;
		}

	}
}

void readSdCard(char *fname,unsigned int x,unsigned int y){

	BYTE res;
	FATFS fs;

	res = disk_initialize();

	if (res == FR_OK)
	{
		res = pf_mount(&fs);


		if (res == FR_OK)
		{
			res = bmpFile(fname);

			if (res == FR_OK)
			{
				fs.flag |= FA_STREAM;      				// Set stream mode
				bmpShow(x, y);
				fs.flag &= ~FA_STREAM;     			// Clear stream mode
			}

		}
	}

	pf_mount(0);
}

/*
 * alarm
 */
void alarm(unsigned char a_h, unsigned char a_m, unsigned char a_s,date_time_t *dt) {

	if (dt->hours == a_h && dt->minutes == a_m && dt->seconds == a_s ) {

		PORTF &= ~_BV(PINF3);
		_delay_ms(380);
		PORTF |= _BV(PINF3);
		_delay_ms(1000);

	}

}

void init(void) {

	sei();
	#ifdef DEBUG
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	uart_puts("\r\nstart");
	#endif
	i2c_init();
    LCD_Reset();
    LCD_Clear(BLACK);
}

//PD6 TOUCH CS
//PB0 MMC CS
//PB7 TSOP
//PF6 LED
//PF5 LCD BL
//PF3 ALARM
//PF0 TEMP
//PORTA LCD BUS
//PORTC LCD BUS
//PORTE LCD CTRL
/**
 * MMC
 * CS PB0
 * TOUCH
 * CS PD6
 */
int main(void) {

	DDRA=0xFF;
	PORTA=0xFF;
	DDRC=0xFF;
	PORTC=0xFF;
	DDRE=0xFF;
	PORTE=0xFF;

//	DDRD=_BV(PIND6);
//	PORTD=_BV(PIND6);

	DDRB=0x0F;
	PORTB=0x0F;

	DDRF=_BV(PINF3)|_BV(PINF6);
	PORTF|=_BV(PINF3)|_BV(PINF6);
	//PORTF&=~_BV(PINF6);


	date_time_t dt;
	float temp;
	char *daynames[] = {"Niedziela","Poniedzialek","Wtorek","Sroda","Czwartek","Piatek","Sobota"};
	char h[4],m[4],dd[4],mm[4],yy[8],tempC[6];
	init();
	unsigned char lastMinute=99;
	unsigned char lastHour=99;
	unsigned char lastDate=99;
	float lastTemp=128;
	unsigned char vlalarm = 0;

    dt.month   = 3;    // December
    dt.day     = 23;    // 31
    dt.year    = 14;    // 2006
    dt.hours   = 14;    // 23 hours
    dt.minutes = 58;    // 59 minutes
    dt.seconds = 15;    // 50 seconds
    dt.weekday = 0;     // 0 = Sunday, 1 = Monday, etc.
    //set bg
	readSdCard("BG.BMP",0,0);

//	PCF8563_set_time(&dt);
	temp = readTemp();

	while (1) {
			temp = readTemp();
			dtostrf(temp,5,1,tempC);
		#ifdef DEBUG
		uart_puts("\r\nloop");
		#endif

		PCF8563_get_time(&dt);

//		if(dt.seconds<10)
//		{
//			itoa(dt.seconds,buffer,10);
//			sprintf(s,"0%s",buffer);
//		}else{
//			itoa(dt.seconds,s,10);
//		}


		//czas
		if(lastHour!=dt.hours)
		{
			if(dt.hours<10)
			{
				itoa(dt.hours,buffer,10);
				sprintf(h,"0%s",buffer);
			}else{
				itoa(dt.hours,h,10);
			}

			LCD_DrawSymbol(75,33,68,68, image_hour);
			lastHour=dt.hours;
		    LcdFont(fn_big);
			SetFgColor(BLACK);
			snprintf(buffer, sizeof buffer, "%s", h);
			DrawText(50,70, 160, 70, buffer, ALINE_CENTER);

		}
		if(lastMinute!=dt.minutes)
		{
			if(dt.minutes<10)
			{
				itoa(dt.minutes,buffer,10);
				sprintf(m,"0%s",buffer);
			}else{
				itoa(dt.minutes,m,10);
			}

			LCD_DrawSymbol(185,33,68,68, image_min);
			lastMinute=dt.minutes;
		    LcdFont(fn_big);
			SetFgColor(BLACK);
			snprintf(buffer, sizeof buffer, "%s", m);
			DrawText(160,70, GetMaxX()-50, 70, buffer, ALINE_CENTER);

		}

		//temp
		if(lastTemp!=temp){

			LCD_DrawSymbol(156,123,129,72, image_temp);
			lastTemp=temp;
			SetFgColor(WHITE);
			LcdFont(fn_big);
		    snprintf(buffer, sizeof buffer, "%s@C ", tempC);
			DrawText(0,150, GetMaxX(), 150, buffer, ALINE_RIGHT);
		}

		//data
		if(lastDate!=dt.day){
			if(dt.day<10)
			{
				itoa(dt.day,buffer,10);
				sprintf(dd,"0%s",buffer);
			}else{
				itoa(dt.day,dd,10);
			}
			if(dt.month<10)
			{
				itoa(dt.month,buffer,10);
				sprintf(mm,"0%s",buffer);
			}else{
				itoa(dt.month,mm,10);
			}
			LCD_DrawSymbol(8,203,306,28, image_btm);
			lastDate=dt.day;
			SetFgColor(WHITE);
			LcdFont(ft_med);
			itoa(dt.year,yy,10);
			snprintf(buffer, sizeof buffer, "%s-%s-%s %s", dd, mm, yy,daynames[dt.weekday]);
			DrawText(0,220, GetMaxX(), 220, buffer, ALINE_CENTER);
			vlalarm=0;
		}

		if(dt.voltagelow==1&&vlalarm==0){
			SetFgColor(WHITE);
			LcdFont(ft_med);
			SetColor(RED);
			FillRectangle(0,200,GetMaxX(),GetMaxY());
			DrawText(0,220, GetMaxX(), 220, "battery voltage is low", ALINE_CENTER);
			vlalarm=1;
		}

//		unsigned int suma = 0;
//		char t[256];
//		memset(&t[0], 0, sizeof(t));
//
//		getIR();
//
//		for (int var = 0; var < h; ++var) {
//
//			itoa(bignal[var], buffer, 10);
//			uart_puts(buffer);
//			strcat(t,buffer);
//			suma += bignal[var];
//		}
//		uart_puts(" EOL ");
//		uart_puts(t);
//		uart_puts(" EOL ");
//		if(strcmp(t,"001000000000001000000110100000000101111001011000")==0)
//		{
//				uart_puts(" RED");
//					PORTD^=_BV(PORTD4);
//
//		}
//
//		alarm(23, 0, 0,&dt);
//		alarm(5, 0, 0,&dt);
//		_delay_ms(500);
	};

	return (0);
}

