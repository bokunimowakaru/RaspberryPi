/**************************************************************************************
UART for Raspberry Pi

                                                  Copyright (c) 2015-2017 Wataru KUNINO
***************************************************************************************/
#include <stdint.h>
int open_serial_port(int speed,char *port);
char getch_serial_port();
int putch_serial_port(char c);
int puts_serial_port(char *s);
int putb_serial_port(uint8_t *in,int len);
int close_serial_port();
