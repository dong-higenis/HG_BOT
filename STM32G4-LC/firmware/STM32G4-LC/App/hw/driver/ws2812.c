/*
 * ws2812.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "ws2812.h"

//	160MHz Clock > 1.3uS
//	High	0.8us
//	Low		0.45us
#define BIT_PERIOD      (207)
#define BIT_HIGH        (127)
#define BIT_LOW         (71)

bool is_init = false;

typedef struct
{
	uint16_t led_cnt;
} ws2812_t;

#define DELAY	200	//	200us Reset code

static uint8_t led_buf[DELAY + 24*64];

ws2812_t ws2812;
extern TIM_HandleTypeDef htim17;

bool ws2812Init(void)
{
	memset(led_buf, 0, sizeof(led_buf));
	is_init = true;
	return true;
}

void ws2812Begin(uint32_t led_cnt)
{
	ws2812.led_cnt = led_cnt;
	HAL_TIM_PWM_Start_DMA(&htim17, TIM_CHANNEL_1, (uint32_t *)led_buf, (DELAY + 24 * led_cnt) * 1);
}

void ws2812SetColor(uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t r_bit[8];
	uint8_t g_bit[8];
	uint8_t b_bit[8];

	uint32_t offset;

	for (int i=0; i<8; i++)
	{
		if (red & (1<<7))
		{
			r_bit[i] = BIT_HIGH;
		}
		else
		{
			r_bit[i] = BIT_LOW;
		}
		red <<= 1;

		if (green & (1<<7))
		{
			g_bit[i] = BIT_HIGH;
		}
		else
		{
			g_bit[i] = BIT_LOW;
		}
		green <<= 1;

		if (blue & (1<<7))
		{
			b_bit[i] = BIT_HIGH;
		}
		else
		{
			b_bit[i] = BIT_LOW;
		}
		blue <<= 1;
	}

	offset = DELAY;

	memcpy(&led_buf[offset + index*24 + 8*0], g_bit, 8*1);
	memcpy(&led_buf[offset + index*24 + 8*1], r_bit, 8*1);
	memcpy(&led_buf[offset + index*24 + 8*2], b_bit, 8*1);
}
