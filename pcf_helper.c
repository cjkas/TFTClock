/*
 * pcf_helper.c
 *
 *  Created on: 20-03-2011
 *      Author: slaw
 */
#include "twimaster.c"
#include "pcf_helper.h"
#ifdef DEBUG
#include "uart.h"
#endif
// This function converts an 8 bit binary value
// to an 8 bit BCD value.
// The input range must be from 0 to 99.
unsigned char bin2bcd(unsigned char bin) {
	return (((bin / 10) << 4) | (bin % 10));
}

// This function converts an 8 bit BCD value to
// an 8 bit binary value.
// The input range must be from 00 to 99.
unsigned char bcd2bin(unsigned char bcd) {
	return (10 * (bcd >> 4) + (bcd & 0x0f));
}

void PCF8563_write_byte(unsigned char address, unsigned char data) {
	char result;
	result = i2c_start(RTCR + I2C_WRITE);
	if (!result) {
		i2c_write(address);
		i2c_write(data);
	}
	i2c_stop();
}

void PCF8563_set_time(date_time_t *dt) {

	char result;
	unsigned char bcd_sec;
	unsigned char bcd_min;
	unsigned char bcd_hrs;
	unsigned char bcd_day;
	unsigned char bcd_mon;
//	unsigned char bcd_weekday;
	unsigned char bcd_year;

	// Convert the input date/time into BCD values
	// that are formatted for the PCF8563 registers.
	bcd_sec = bin2bcd(dt->seconds);
	bcd_min = bin2bcd(dt->minutes);
	bcd_hrs = bin2bcd(dt->hours);
	bcd_day = bin2bcd(dt->day);
	bcd_mon = bin2bcd(dt->month);
	bcd_year = bin2bcd(dt->year);
	
	result = i2c_start(RTCR + I2C_WRITE);
	if (!result) {
		i2c_write(PCF8563_CTRL_STATUS_REG1);
		i2c_write(PCF8563_STOP_COUNTING);
	}
	///2
	result = i2c_rep_start(RTCR + I2C_WRITE);
	if (!result) {
		i2c_write(PCF8563_SECONDS_REG);
		i2c_write(bcd_sec);
		i2c_write(bcd_min);
		i2c_write(bcd_hrs);
		i2c_write(bcd_day);
		i2c_write(dt->weekday);
		i2c_write(bcd_mon);
		i2c_write(bcd_year);
	}
	result = i2c_rep_start(RTCR + I2C_WRITE);
	if (!result) {
		i2c_write(PCF8563_CTRL_STATUS_REG1);
		i2c_write(PCF8563_START_COUNTING);
	}
	i2c_stop();

}

void PCF8563_get_time(date_time_t *dt) {

	char result;

	unsigned char bcd_sec = 0;
	unsigned char bcd_min = 0;
	unsigned char bcd_hrs = 0;
	unsigned char bcd_day = 0;
	unsigned char bcd_mon = 0;
	unsigned char bcd_wday = 0;
	unsigned char bcd_year = 0;

	result = i2c_start(RTCR + I2C_WRITE);
	if (!result) {
		i2c_write(PCF8563_SECONDS_REG);
		result = i2c_rep_start(RTCR + I2C_READ);
		if (!result) {
			bcd_sec = i2c_readAck();
			bcd_min = i2c_readAck();
			bcd_hrs = i2c_readAck();
			bcd_day = i2c_readAck();
			bcd_wday = i2c_readAck();
			bcd_mon = i2c_readAck();
			bcd_year = i2c_readNak();
		}

		if (bcd_sec & 0x80) {
				#ifdef DEBUG
				uart_puts ("### Warning: RTC Low Voltage - date/time not reliable\n");
				#endif
				dt->voltagelow=1;
		}else{
			dt->voltagelow=0;
		}

		dt->seconds = bcd2bin(bcd_sec & 0x7F);
		dt->minutes = bcd2bin(bcd_min & 0x7F);
		dt->hours = bcd2bin(bcd_hrs  & 0x3F);
		dt->day = bcd2bin(bcd_day  & 0x3F);
		dt->weekday = bcd2bin(bcd_wday & 0x07);
		dt->month = bcd2bin(bcd_mon & 0x1F);
		dt->year = bcd2bin(bcd_year);


	}

}

