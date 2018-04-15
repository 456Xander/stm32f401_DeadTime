#include "delay.h"

extern u32 SystemCoreClock;

#define DEL_TIM TIM3

#define I(x) x

#define DEL_TIM_ISR(FN_NAME) I(void) FN_NAME##_IRQHandler(){ \
		if(DEL_TIM->SR & TIM_SR_UIF) {\
			DEL_TIM->DIER &= ~TIM_DIER_UIE;\
		}\
		NVIC_ClearPendingIRQ(FN_NAME##_IRQn); \
		return; \
	}

DEL_TIM_ISR(TIM3)

Delay del_init(TIM_TypeDef *Timer) {
	Delay d;

	d.Timer = Timer;
	return d;
}
void del_changeTimeBase(Delay *del, u16 timebase) {
	del->Timer->PSC = timebase;
}

int del_changeTimeBaseMs(Delay *del, u16 maxTimeMillis) {
	uint32_t largeNum = maxTimeMillis * (SystemCoreClock / 1000);
	u32 psc = largeNum >> 16;
	psc++;
	if (psc > 0xFFFF) {
		del->Timer->PSC = 0xFFFF;
		return -1;
	} else {
		del->Timer->PSC = psc;
	}
	return 0;
}

int del_changeTimeBaseUs(Delay *del, u16 maxTimeMicros) {
	uint64_t largeNum = ((uint64_t) maxTimeMicros) * SystemCoreClock;
	largeNum /= 1000000; //convert from us
	u32 psc = largeNum >> 16;
	//make sure we have a timing long enough:
	psc++;
	if (psc > 0xFFFF) {
		del->Timer->PSC = 0xFFFF;
		return -1;
	} else {
		del->Timer->PSC = psc;
	}
	return 0;
}

static inline void doDelay(Delay *delay) {
	delay->Timer->CNT = 0;
	delay->Timer->SR &= ~TIM_SR_UIF;
	delay->Timer->CR1 |= TIM_CR1_CEN;
	while (!(delay->Timer->SR & TIM_SR_UIF))
		;
	delay->Timer->CR1 &= ~TIM_CR1_CEN;
}

void delay_ms(Delay *delay, u16 ms) {
	delay->Timer->ARR = ((u32) ((SystemCoreClock / delay->Timer->PSC) * ms))
			/ 1000;

	doDelay(delay);
}
void delay_us(Delay *delay, u16 us) {
	delay->Timer->ARR =
			((u32) ((SystemCoreClock / (delay->Timer->PSC + 1)) * us))
					/ 1000000;
	doDelay(delay);
}

