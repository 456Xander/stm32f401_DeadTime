/*
 * delay.h
 *
 *  Created on: 11.03.2018
 *      Author: alex
 */

#ifndef DELAY_H_
#define DELAY_H_
#include "stm32f4xx.h"

typedef struct {
	TIM_TypeDef *Timer;
	void (*delay_ms)(void);
	void (*delay_us)(void);
} Delay;

Delay del_init(TIM_TypeDef *TIMx);
void del_changeTimeBase(Delay *del, u16 timebase);
int del_changeTimeBaseUs(Delay *del, u16 maxTimeMicros);
int del_changeTimeBaseMs(Delay *del, u16 maxTimeMillis);
void delay_ms(Delay *delay, u16 ms);
void delay_us(Delay *delay, u16 us);

#endif /* DELAY_H_ */
