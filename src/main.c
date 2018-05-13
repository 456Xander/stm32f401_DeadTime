/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#define SERIAL_USE_STDIO

#include "stm32f4xx.h"
#include "init.h"
#include "delay.h"
#include "ringbuffer.h"
#include "serial.h"
#include "util.h"
#define TIM1_CH1_IDLE  do { uint16_t ccmr = TIM1->CCMR1; ccmr |= TIM_CCMR1_OC1M_0; ccmr &= ~TIM_CCMR1_OC1M_1; TIM1->CCMR1 = ccmr;} while(0)
#define TIM1_CH1_RUN do { uint16_t ccmr = TIM1->CCMR1; ccmr &= ~TIM_CCMR1_OC1M_0; ccmr |= TIM_CCMR1_OC1M_1; TIM1->CCMR1 = ccmr;} while(0)

/*
 =======================================================
 -------------------Pin Description---------------------
 =======================================================

 CH1 -> PA8
 CH1N -> PB13

 CH2 -> PA9
 CH2N -> PB14

 */

__IO uint32_t *tim_ccr = &TIM1->CCR1;

static inline void forward() {
	TIM1->CCR1 = *tim_ccr;
	tim_ccr = &TIM1->CCR1;
	M_OUT(GPIOA, 9);
	GPIOA->MODER = 0xa8040000;
	GPIOA->BSRRH = 1 << 9;
	M_OUT(GPIOB, 14);
	GPIOB->BSRRL = 1 << 14;

	M_ALT(GPIOA, 8);
	M_ALT(GPIOB, 13);

}

static inline void backward() {
	TIM1->CCR2 = *tim_ccr;
	tim_ccr = &TIM1->CCR2;
	M_OUT(GPIOA, 8);
	GPIOA->BSRRH = 1 << 8;
	M_OUT(GPIOB, 13);
	GPIOB->BSRRL = 1 << 13;

	M_ALT(GPIOA, 9);
	M_ALT(GPIOB, 14);
}

static inline void brake() {
	M_OUT(GPIOA, 8);
	M_OUT(GPIOA, 9);
	GPIOA->BSRRH = 1 << 8 | 1 << 9;
	M_OUT(GPIOB, 13);
	M_OUT(GPIOB, 14);
	GPIOB->BSRRL = 1 << 13 | 1 << 14;
}

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	tim_ccr = &TIM1->CCR1;
	initGPIO();
	initTim1();
	forward();
	initTim2();
//	initTim3();
//	serial_init(64, 32);

//	Delay del = del_init(TIM3);
//	del_changeTimeBaseMs(&del, 20000);

	for (;;) {
//		TIM1_CH1_RUN
//		;
//		delay_ms(&del, 5000);
//		TIM1_CH1_IDLE
//		;
//		delay_ms(&del, 5000);

	}
}

void TIM2_IRQHandler() {
	static int16_t ccr = 50;
	static int mode = 0;
	if (TIM2->SR & TIM_SR_UIF) {
		ccr++;
		if (ccr >= 1500) {
			brake();
			ccr = -50;
		}
		if (ccr == 0) {
			if (!mode) {
				backward();
				mode = 1;
			} else {
				forward();
				mode = 0;
			}
		}
		*tim_ccr = ccr > 1000 ? 1000 : ccr;
		TIM2->SR &= ~TIM_SR_UIF;
	} else if (TIM2->SR & TIM_SR_CC1IF) {

		TIM2->SR &= ~TIM_SR_CC1IF;
	}
}

void HardFault_Handler() {
	GPIOC->ODR = 0x0000;
	while (1)
		;
}

void TIM1_CC_IRQHandler() {
	GPIOC->ODR ^= 0x1;
}
