/*
 * ws2812.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "ws2812.h"
#include "cli.h"


#ifdef _USE_HW_NEOPIXEL

//	160MHz Clock > 1.3uS
//	High	0.7us
//	Low		0.35us
#define BIT_PERIOD      (207)
#define BIT_HIGH        (127)
#define BIT_LOW         (71)

#define DELAY	200	//	200us Reset code value

bool is_init = false;

typedef struct
{
	uint16_t led_cnt;
	uint8_t led_buf[DELAY + 24*HW_NEOPIXEL_MAX_COUNT];
} ws2812_t;


ws2812_t ws2812[HW_NEOPIXEL_MAX_CH];

extern TIM_HandleTypeDef htim17;	//	NeoPixel 0
extern TIM_HandleTypeDef htim16;	//	NeoPixel 1

//	CLI function
#ifdef _USE_HW_CLI
void cliNeopixel(cli_args_t *args);
#endif

bool ws2812Init(void)
{
	memset(ws2812[0].led_buf, 0, sizeof(ws2812[0].led_buf));
	memset(ws2812[1].led_buf, 0, sizeof(ws2812[1].led_buf));

#ifdef _USE_HW_CLI
	cliAdd("neopixel", cliNeopixel);
#endif

	is_init = true;
	return true;
}

void ws2812Begin(uint32_t led_cnt)
{
	ws2812[0].led_cnt = led_cnt;
	ws2812[1].led_cnt = led_cnt;
	//	DMA Start
	HAL_TIM_PWM_Start_DMA(&htim17, TIM_CHANNEL_1, (uint32_t *)ws2812[0].led_buf, (DELAY + 24 * ws2812[0].led_cnt) * 1);
	HAL_TIM_PWM_Start_DMA(&htim16, TIM_CHANNEL_1, (uint32_t *)ws2812[1].led_buf, (DELAY + 24 * ws2812[1].led_cnt) * 1);
}

void ws2812SetColor(uint32_t ch, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t r_bit[8];
	uint8_t g_bit[8];
	uint8_t b_bit[8];

	uint32_t offset;
	if(ch >= HW_NEOPIXEL_MAX_CH || index >= HW_NEOPIXEL_MAX_COUNT)
	{
		return;
	}

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
	//	Xus delay value
	offset = DELAY;

	memcpy(&ws2812[ch].led_buf[offset + index*24 + 8*0], g_bit, 8*1);
	memcpy(&ws2812[ch].led_buf[offset + index*24 + 8*1], r_bit, 8*1);
	memcpy(&ws2812[ch].led_buf[offset + index*24 + 8*2], b_bit, 8*1);	
}

#ifdef _USE_HW_CLI
void cliNeopixel(cli_args_t *args)
{
	bool ret = false;
	uint8_t	ch;
	uint8_t r, g, b;
	uint8_t index;

	if (args->argc == 1 && args->isStr(0, "info") == true)
	{
		cliPrintf("neopixel init: %d\n", is_init);	
		for(uint8_t i=0;i<HW_NEOPIXEL_MAX_CH;i++)
		{
			cliPrintf("[%d] max leds: %d\n", i, ws2812[i].led_cnt);
		}			
		ret = true;
	}
	if (args->argc == 6 && args->isStr(0, "set") == true)
	{
		ch  = (uint8_t)args->getData(1);
		index = (uint8_t)args->getData(2);
		r = (uint8_t)args->getData(3);
		g = (uint8_t)args->getData(4);
		b = (uint8_t)args->getData(5);

		if(ch < HW_NEOPIXEL_MAX_CH)
		{		
			if(index < ws2812[ch].led_cnt)
			{
				ws2812SetColor(ch, index, r, g, b);
				cliPrintf("ch=%d, index=%d, r=%d, g=%d, b=%d\n", ch, index, r, g, b);
				ret = true;
			}
		}
	}
	if (ret == false)
	{
		cliPrintf("noepixel info\n");    
		cliPrintf("noepixel set [ch] [index] [red] [gree] [blue]\n");    
	}
}

#endif

#endif
