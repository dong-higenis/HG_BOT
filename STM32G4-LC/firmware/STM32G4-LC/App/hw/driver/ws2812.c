/*
 * ws2812.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "ws2812.h"
#include "cli.h"

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
} ws2812_t;

static uint8_t neo_pixel_0ch_led_buf[DELAY + 24*64];
static uint8_t neo_pixel_1ch_led_buf[DELAY + 24*64];

ws2812_t ws2812;
extern TIM_HandleTypeDef htim17;	//	NeoPixel 0
extern TIM_HandleTypeDef htim16;	//	NeoPixel 1

//	CLI function
#ifdef _USE_HW_CLI
void cliNeopixel(cli_args_t *args);
#endif

bool ws2812Init(void)
{
	memset(neo_pixel_0ch_led_buf, 0, sizeof(neo_pixel_0ch_led_buf));
	memset(neo_pixel_1ch_led_buf, 0, sizeof(neo_pixel_1ch_led_buf));

#ifdef _USE_HW_CLI
	cliAdd("neopixel", cliNeopixel);
#endif

	is_init = true;
	return true;
}

void ws2812Begin(uint32_t led_cnt)
{
	ws2812.led_cnt = led_cnt;
	//	DMA Start
	HAL_TIM_PWM_Start_DMA(&htim17, TIM_CHANNEL_1, (uint32_t *)neo_pixel_0ch_led_buf, (DELAY + 24 * led_cnt) * 1);
	HAL_TIM_PWM_Start_DMA(&htim16, TIM_CHANNEL_1, (uint32_t *)neo_pixel_1ch_led_buf, (DELAY + 24 * led_cnt) * 1);
}

void ws2812SetColor(uint32_t ch, uint32_t index, uint8_t red, uint8_t green, uint8_t blue)
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

	//	Xus delay value
	offset = DELAY;

	switch(ch)
	{
	case 0:
		memcpy(&neo_pixel_0ch_led_buf[offset + index*24 + 8*0], g_bit, 8*1);
		memcpy(&neo_pixel_0ch_led_buf[offset + index*24 + 8*1], r_bit, 8*1);
		memcpy(&neo_pixel_0ch_led_buf[offset + index*24 + 8*2], b_bit, 8*1);
		break;

	case 1:
		memcpy(&neo_pixel_1ch_led_buf[offset + index*24 + 8*0], g_bit, 8*1);
		memcpy(&neo_pixel_1ch_led_buf[offset + index*24 + 8*1], r_bit, 8*1);
		memcpy(&neo_pixel_1ch_led_buf[offset + index*24 + 8*2], b_bit, 8*1);
		break;
	}
}

#ifdef _USE_HW_CLI
void cliNeopixel(cli_args_t *args)
{
	bool ret = true;
	uint8_t	ch;
	uint8_t r, g, b;
	uint8_t led_number;

	//	neopixel <ch> <index> <R> <G> <B>
	cliPrintf("args->argc = %d\n",args->argc);
	if (args->argc == 5)
	{
		ch  = (uint8_t)args->getData(0);
		led_number = (uint8_t)args->getData(1);
		r = (uint8_t)args->getData(2);
		g = (uint8_t)args->getData(3);
		b = (uint8_t)args->getData(4);

		//	LED Value Limit (Protect overflow)
		if(r >= 255)
		{
			r = 255;
		}
		else if(r < 0)
    {
      r = 0;
    }

		if(g >= 255)
		{
			g = 255;
		}
    else if(g < 0)
    {
      g = 0;
    }

		if(b >= 255)
		{
			b = 255;
		}
    else if(b < 0)
    {
      b = 0;
    }

		//	Limit LED count
		if(led_number >= ws2812.led_cnt - 1)
		{
			led_number = ws2812.led_cnt - 1;
		}

		//
		cliPrintf("ch=%d, led_number=%d, r=%d, g=%d, b=%d\n", ch, led_number, r, g, b);
		ws2812SetColor(ch, led_number, r, g, b);
	}
	else
	{
		ret = false;
	}

	if (ret == false)
	{
		cliPrintf("Command format => NEOPIXEL <led ch> <led index> <red value> <green value> <blue value>\n");
		cliPrintf("ex) NEOPIXEL 0 0 255 255 255  >> white out\n");
		cliPrintf("ex) NEOPIXEL 0 0 255 0 0  >> red out\n");
		cliPrintf("ex) NEOPIXEL 0 0 0 255 0  >> green out\n");
		cliPrintf("ex) NEOPIXEL 0 0 0 0 255  >> blue out\n");
	}
}
#endif
