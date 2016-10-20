/*
 * pcf_helper.h
 *
 *  Created on: 20-03-2011
 *      Author: slaw
 */

#ifndef PCF_HELPER_H_
#define PCF_HELPER_H_


#endif /* PCF_HELPER_H_ */

#define PCF8563_CTRL_STATUS_REG1    0x00	//reg1
#define PCF8563_CTRL_STATUS_REG2    0x01 //reg2
#define PCF8563_SECONDS_REG        	0x02 //>1 VL@1
#define PCF8563_MINUTES_REG        	0x03 //>1
#define PCF8563_HOURS_REG          	0x04 //>2
#define PCF8563_DAY_REG	           	0x05 //>2
#define PCF8563_WEEKDAYS_REG       	0x06 //>>5
#define PCF8563_MONTHS_REG         	0x07 //>>3 C@1
#define PCF8563_YEAR_REG           	0x08

#define PCF8563_ALARM_MINS_REG     	0x09 //>1 AEM@1
#define PCF8563_ALARM_HOURS_REG    	0x0A //>2 AEH@1
#define PCF8563_ALARM_DAY_REG     	0x0B //>2 AED@1
#define PCF8563_ALARM_WEEKDAY		0x0F //>5 AEW@1

// Commands for the Control/Status register.
#define PCF8563_START_COUNTING     0x00
#define PCF8563_STOP_COUNTING      0x40

#define RTCR 0xA2
#define RTCW 0xA3

typedef struct
{
unsigned char  seconds;    // 0 to 59
unsigned char  minutes;    // 0 to 59
unsigned char  hours;      // 0 to 23  (24-hour time)
unsigned char  day;        // 1 to 31
unsigned char  month;      // 1 to 12
unsigned char  year;       // 00 to 99
unsigned char  weekday;    // 0 = Sunday, 1 = Monday, etc.
unsigned char  voltagelow;	//0 is ok  , 1 is low
}date_time_t;

void PCF8563_set_time(date_time_t *dt);
void PCF8563_get_time(date_time_t *dt);
