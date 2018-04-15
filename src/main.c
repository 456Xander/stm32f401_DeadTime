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

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();

	initGPIO();
	initTim1();
	initTim2();
	initTim3();
//	serial_init(64, 32);

	Delay del = del_init(TIM3);
	del_changeTimeBaseMs(&del, 50);
	uint16_t ccr = TIM1->CCR1;
	for (;;) {

		delay_ms(&del, 10);
		if (ccr >= 500) {
			ccr = 0;
		}
		TIM1->CCR1 = ccr;
	}
}

void TIM2_IRQHandler() {
	if (TIM2->SR & TIM_SR_UIF) {

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
