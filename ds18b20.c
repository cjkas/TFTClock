/*
 * ds18b20.c
 *
 *  Created on: 20-03-2011
 *      Author: slaw
 */
#include <avr/io.h>
#include <util/delay.h>
#include "ds18b20.h"

unsigned char RESET_PULSE(void) {
	unsigned char PRESENCE;
	CLEAR_1wire; // ustawienie na poziom niski
	_delay_us(500);
	SET_1wire; //ustawienie na poziom wysoki
	_delay_us(60); //oczekiwanie na ustaweinie linii w poziom niski przez ds'a

	//sprawdzenie poziomu na linii ds'a czy w stanie niskim
	if (bit_is_clear(PORT_1wire, WE)) {PRESENCE=1;} else {PRESENCE=0;}
	//1-odebrano bit presence, 0 stan nieaktywnosci
	_delay_us(500);// oczekiwanie przez mastera ok 470 mikro sekund i spr czy ds podciagnal magistrale
	//sprawdzamy czy poziom linni ustawiony
	if (bit_is_set(PORT_1wire, WE)) {PRESENCE=1;} else {PRESENCE=0;}
	return PRESENCE; // zwracamy wartosc do funkcji
}

//wysyla do ukladu pojedynczy bit
void send(char bit) {
	CLEAR_1wire;// ustawienie w stan niski magistrali
	_delay_us(2);
	if (bit == 1)
		SET_1wire; //zwolnienie magistrali - wyslanie jedynki
	_delay_us(60);//przetrzymanie - wyslanie zera
	SET_1wire;
}

//odczytuje bit z magistrali
unsigned char read(void) {
	unsigned char PRESENCE = 0;

	CLEAR_1wire; //ustawienie w stan niski
	_delay_us(3);
	SET_1wire;
	_delay_us(3);

	if (bit_is_set(PORT_1wire, WE))
	{	PRESENCE = 1;}
	else
		{PRESENCE = 0;}
	_delay_us(55);
	return (PRESENCE);
}

//wysyla caly bajt do ukladu

void send_byte(char wartosc) {
	unsigned char i;//zmienna licznikowa
	unsigned char pom; //zmienna pomocnicza

	for (i = 0; i < 8; i++) {
		pom = wartosc >> i;//przesuniecie bitowe w prawo
		pom &= 0x01; //skopiowanie bitu do zmiennej pomocniczej
		send(pom); //wyslanie bitu na magistrale
		_delay_us(2);
	}
}

//zczytuje caly bajt z ukladu
unsigned char read_byte(void) {
	unsigned char i;// zmienna licznikowa
	unsigned char wartosc = 0; //zczytywana wartosc

	for (i = 0; i < 8; i++) {
		if (read())
			wartosc |= 0x01 << i; //zczytywanie po jednym bicie
	}

	return (wartosc);
}

float readTemp(void) {

	float temp=0;
	char lsb, msb;

		if (RESET_PULSE()) {
			send_byte(0xCC); //skip rom
			send_byte(0x44);//convert t
			//			_delay_ms(750);

			if (RESET_PULSE()) {
				send_byte(0xCC);//skip rom
				send_byte(0xBE); //read scratchpad

				lsb = read_byte();//odczytanie lsb
				msb = read_byte();//odczytanie msb

				RESET_PULSE(); //zowlnienie magistrali
				temp = msb << 8 | lsb ;
				temp/=16;
			}
		}

		return temp;

}
