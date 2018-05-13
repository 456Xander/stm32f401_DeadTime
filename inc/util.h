/*
 * util.h
 *
 *  Created on: 13.05.2018
 *      Author: alex
 */

#ifndef UTIL_H_
#define UTIL_H_

#define M_ALT(gpio, x) do{uint32_t moder = gpio->MODER;moder &= ~(0x3 << 2*x) ; moder |= 0x2 << (2*x); gpio->MODER = moder;}while(0)
#define M_OUT(gpio, x) do{uint32_t moder = gpio->MODER;moder &= ~(0x3 << 2*x) ; moder |= 0x1 << (2*x); gpio->MODER = moder;}while(0)

#endif /* UTIL_H_ */
