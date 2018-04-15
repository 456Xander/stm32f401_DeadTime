#include "serial.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include "ringbuffer.h"

RingBuffer send;
RingBuffer recv;

int line; // if a newline was read
uint8_t hex_to_ascii[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F' };
int pause = 0;

#define SEND if(!pause) USART2->CR1 |= USART_CR1_TXEIE

void serial_init(int size_send, int size_recv) {
	initRingBuffer(&send, size_send);
	initRingBuffer(&recv, size_recv);

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	uint16_t cr1 = USART2->CR1;
	cr1 |= USART_CR1_UE;
	cr1 |= USART_CR1_TXEIE;
	cr1 |= USART_CR1_RXNEIE;
	cr1 |= USART_CR1_TCIE;
	cr1 |= USART_CR1_TE;
	cr1 |= USART_CR1_RE;
	USART2->CR1 = cr1;
	USART2->BRR = 0x016D;

	NVIC_EnableIRQ(USART2_IRQn);
}

void serial_pause() {
	pause = 1;
	USART2->CR1 &= ~USART_CR1_TXEIE;
}

void serial_resume() {
	pause = 0;
	SEND;
}

void serial_write(uint8_t c) {
	ringBufferAppend(&send, c);
	SEND;
}

void serial_newline() {
	serial_print("\r\n");
}

void serial_print_hex8(uint8_t number) {
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF0) >> 4]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF)]);
	SEND;
}

void serial_print_hex16(uint16_t number) {
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF000) >> 12]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF00) >> 8]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF0) >> 4]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF)]);

	SEND;
}

void serial_print_hex32(uint32_t number) {
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF0000000) >> 28]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF000000) >> 24]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF00000) >> 20]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF0000) >> 16]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF000) >> 12]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF00) >> 8]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF0) >> 4]);
	ringBufferAppend(&send, hex_to_ascii[(number & 0xF)]);
	SEND;
}

int serial_print(char *str) {
	int ret = ringBufferAppendStr(&send, str);
	SEND;
	return ret;
}

int serial_available() {
	return recv.waitingData;
}
uint8_t serial_read() {
	return ringBufferGet(&recv);
}
int serial_readStr(char *out, int maxLen, char terminator) {
	int i;
	for (i = 0; i < maxLen && recv.waitingData; i++) {
		*out = ringBufferGet(&recv);
		if (*out == terminator || recv.waitingData == 0) {
			break;
		}
		out++;
	}
	*out = '\0';

	return i;
}

void USART2_IRQHandler() {
	if (USART2->SR & USART_SR_TXE) {
		if (send.waitingData) {
			USART2->DR = ringBufferGet(&send);
		} else {
			USART2->CR1 &= ~USART_CR1_TXEIE;
		}
	}
	if (USART2->SR & USART_SR_RXNE) {
		uint8_t data = USART2->DR;
		if (data == '\n')
			line = 1;
		ringBufferAppend(&recv, data);
	}
	if (USART2->SR & USART_SR_TC) {
		USART2->CR1 &= ~USART_CR1_TCIE;
	}

//	NVIC_ClearPendingIRQ(USART2_IRQn);
}

int serial_readLine(char *out, int maxLen) {
	if (!line) {
		return -1;
	}
	int i;
	for (i = 0; i < maxLen - 1; i++) {
		out[i] = ringBufferGet(&recv);
		if (out[i] == '\n') {
			i--;
			break;
		}
	}
	out[i + 1] = '\0';
	line = 0;
	return i;
}

int serial_lineAvail() {
	return line;
}

void serial_flushIn() {
	ringBufferClear(&recv);
}

void serial_printf(char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char buf[send.size];
	vsprintf(buf, format, argptr);
	ringBufferAppendN(&send, buf, strlen(buf));
	va_end(argptr);
	SEND;
}
void serial_scanf(char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char buf[recv.waitingData + 1];
	ringBufferGetN(&recv, buf, recv.waitingData);
	buf[recv.waitingData] = '\0';
	vsscanf(buf, format, argptr);
	va_end(argptr);
}
void serial_scanf_line(char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char buf[recv.waitingData + 1];
	serial_readLine(buf, recv.waitingData + 1);
	vsscanf(buf, format, argptr);
	va_end(argptr);
}
