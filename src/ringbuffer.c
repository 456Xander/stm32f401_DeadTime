#include "ringbuffer.h"
#include <stdlib.h>

int initRingBuffer(RingBuffer *buffer, uint16_t size){
	buffer->base = malloc(size);
	buffer->size = size;
	buffer->currentRead = buffer->base;
	buffer->currentWrite = buffer->base;
	if(buffer->base == NULL){
		 return -1;
	}
	return 0;
}

void deinitRingBuffer(RingBuffer *buffer){
	 buffer->size = 0;
	 free(buffer->base);
	 buffer->base = NULL;
	 buffer->currentRead = NULL;
	 buffer->currentWrite = NULL;
}

int ringBufferAppend(RingBuffer *buffer, uint8_t data){
	if(buffer->waitingData >= buffer->size){
		 return -1;
	}
	*(buffer->currentWrite) = data;
	buffer->currentWrite++;
	buffer->waitingData++;
	if(buffer->currentWrite >= buffer->base + buffer->size){
		 buffer->currentWrite = buffer->base;
	}
	return 0;
}

int ringBufferAppendN(RingBuffer *buffer, uint8_t *data, uint8_t len){
	for(int i = 0; i < len; i++){
		if(ringBufferAppend(buffer, data[i]) != 0)
			return -1;
	}
	return 0;
}

int ringBufferAppendStr(RingBuffer *buffer, char *str){
	for(int i = 0; str[i] != '\0'; i++){
		if(ringBufferAppend(buffer, str[i]) != 0)
			return -1;
	}
	return 0;
}

uint8_t ringBufferGet(RingBuffer *buffer){
	if(buffer->waitingData == 0)
		return 0xFF;
	 uint8_t read = *(buffer->currentRead);
	 buffer->currentRead++;
	 buffer->waitingData--;
	 if(buffer->currentRead >= buffer->base + buffer->size){
		  buffer->currentRead = buffer->base;
	 }
	 return read;
}

int ringBufferGetN(RingBuffer *buffer, uint8_t *dataOut, uint8_t len){
	int i = 0;
	 for(i = 0; buffer->waitingData && i < len; i++){
		 dataOut[i]  = ringBufferGet(buffer); 
	 }
	 return i;
}
int ringBufferGetStr(RingBuffer *buffer, char *dataOut, uint8_t len){
	int i = 0;
	 for(i = 0; buffer->waitingData && i < len - 1; i++){
		 dataOut[i]  = ringBufferGet(buffer); 
	 }
	 dataOut[i] = '\0';
	 return i;
}

void ringBufferClear(RingBuffer *buffer){
	buffer->currentRead = buffer->currentWrite;
	buffer->waitingData = 0;
}

