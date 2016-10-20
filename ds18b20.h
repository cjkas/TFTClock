#define WE 0
#define PORT_1wire PINF
#define SET_1wire DDRF&=~_BV(WE)
#define CLEAR_1wire DDRF|=_BV(WE)

unsigned char RESET_PULSE(void);
void send(char bit);
void send_byte(char wartosc);
unsigned char read(void);
unsigned char read_byte(void);
float readTemp(void);
