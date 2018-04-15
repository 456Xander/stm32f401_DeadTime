#pragma once
#include <stdint.h>

typedef struct {
	uint16_t size;
	uint16_t waitingData;
	uint8_t *base;
	uint8_t *currentRead;
	uint8_t *currentWrite;
}RingBuffer;

int initRingBuffer(RingBuffer *buffer, uint16_t size);
void deinitRingBuffer(RingBuffer *buffer);
int ringBufferAppend(RingBuffer *buffer, uint8_t data);
int ringBufferAppendN(RingBuffer *buffer, uint8_t *data, uint8_t len);
int ringBufferAppendStr(RingBuffer *buffer, char *str);
uint8_t ringBufferGet(RingBuffer *buffer);
int ringBufferGetN(RingBuffer *buffer, uint8_t *dataOut, uint8_t len);
int ringBufferGetStr(RingBuffer *buffer, char *dataOut, uint8_t len);

void ringBufferClear(RingBuffer *buffer);
