/*
 * ws2812.h
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */

#ifndef COMMON_HW_INCLUDE_WS2812_H_
#define COMMON_HW_INCLUDE_WS2812_H_

#include "hw_def.h"

bool ws2812Init(void);
void ws2812Begin(uint32_t led_cnt);
void ws2812SetColor(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);

#endif /* COMMON_HW_INCLUDE_WS2812_H_ */
