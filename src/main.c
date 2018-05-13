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

#define TIM1_CH1_IDLE  do { uint16_t ccmr = TIM1->CCMR1; ccmr |= TIM_CCMR1_OC1M_0; ccmr &= ~TIM_CCMR1_OC1M_1; TIM1->CCMR1 = ccmr;} while(0)
#define TIM1_CH1_RUN do { uint16_t ccmr = TIM1->CCMR1; ccmr &= ~TIM_CCMR1_OC1M_0; ccmr |= TIM_CCMR1_OC1M_1; TIM1->CCMR1 = ccmr;} while(0)

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();

	initGPIO();
	initTim1();
//	initTim2();
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
	static uint16_t ccr = 50;

	if (TIM2->SR & TIM_SR_UIF) {
//		ccr++;
		if (ccr >= 1000) {
			ccr = 0;
		}
//		TIM1->CCR1 = ccr;
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
