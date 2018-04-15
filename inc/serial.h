/*
 * uart.h
 *
 *  Created on: 08.04.2018
 *      Author: alex
 */
#include "stm32f4xx.h"


#ifndef SERIAL_H_
#define SERIAL_H_

void serial_init(int size_send, int size_recv);
void serial_write(uint8_t c);
int serial_print(char *str);

int serial_available();
int serial_lineAvail();
uint8_t serial_read();
int serial_readStr(char *out, int maxLen, char terminator);
int serial_readLine(char *out, int maxLen);
uint8_t serial_read_hex8();
uint8_t serial_read_hex16();
uint8_t serial_read_hex32();

void serial_flushIn();
void serial_newline();
void serial_print_hex8(uint8_t number);
void serial_print_hex16(uint16_t number);
void serial_print_hex32(uint32_t number);

void serial_pause();
void serial_resume();

#ifdef SERIAL_USE_STDIO
void serial_printf(char *format, ...);
void serial_scanf(char *format, ...);
void serial_scanf_line(char *format, ...);
#endif

#endif /* SERIAL_H_ */
