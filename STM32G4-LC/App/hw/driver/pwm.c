/*
 * pwm.c
 *
 *  Created on: Sep 25, 2024
 *      Author: user
 */
#include "pwm.h"
#include "cli.h"

#include "tim.h"

//
//	PWM Code Enable
//
#ifdef _USE_HW_PWM

//	Using CLI
#ifdef _USE_HW_CLI
static void cliPwm(cli_args_t *args);
#endif

//
//	PWM driver struct
//
typedef struct
{
	uint16_t max_value;
	uint16_t duty;
	uint32_t channel;
} pwm_tbl_t;
static pwm_tbl_t  pwm_tbl[PWM_MAX_CH];

//	NeoPixel Test
//static uint8_t led_buf[50 + 24*64];

//
//	Global Variable
//
static bool is_init = false;

//
//
//

bool pwmInit(void)
{
	bool ret = true;
	for (int i=0; i<PWM_MAX_CH; i++)
	{
		pwm_tbl[i].duty = 0;
	    pwm_tbl[i].max_value = 65535;
	}

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	//HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);

	//int led_cnt = 1;
	//HAL_TIM_PWM_Start_DMA(&htim16, TIM_CHANNEL_1, (uint32_t *)led_buf, (50 + 24 * led_cnt) * 1 );
	//HAL_TIM_PWM_Start_DMA(&htim17, TIM_CHANNEL_1, (uint32_t *)led_buf, (50 + 24 * led_cnt) * 1 );

#ifdef _USE_HW_CLI
	cliAdd("pwm", cliPwm);
#endif
	is_init = ret;
	return ret;
}

bool pwmIsInit(void)
{
	return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
	int res = 0;

	if (ch >= PWM_MAX_CH) return;
	uint32_t temp_buffer[4] = {0,};

	pwm_tbl[ch].duty = constrain(pwm_data, 0, pwm_tbl[ch].max_value);
	temp_buffer[0] = pwm_tbl[ch].duty;
	temp_buffer[1] = pwm_tbl[ch].duty;
	temp_buffer[2] = pwm_tbl[ch].duty;
	temp_buffer[3] = pwm_tbl[ch].duty;
	//cliPrintf("[%d]set data = %d\n",ch,(uint32_t)pwm_tbl[ch].duty);
	cliPrintf("[%d]set data = %d (org pwm_data = %d)(pwm_tbl[ch].max_value = %d)\n",ch,(uint32_t)pwm_tbl[ch].duty,pwm_data, pwm_tbl[ch].max_value);
	switch(ch)
	{
	//
	//	Servo Motor
	//
	case _DEF_PWM1:
		htim2.Instance->CCR1 = (uint32_t)pwm_tbl[ch].duty;
		cliPrintf("htim2.Instance->CCR1 = %d\n",htim2.Instance->CCR1);
		break;
	case _DEF_PWM2:
		htim3.Instance->CCR2 = (uint32_t)pwm_tbl[ch].duty;
		cliPrintf("htim3.Instance->CCR2 = %d\n",htim3.Instance->CCR2);
		break;

	//
	//	NeoPixel
	//
	case _DEF_PWM3:
		//htim16.Instance->CCR1 = (uint32_t)pwm_tbl[ch].duty;
		//cliPrintf("htim16.Instance->CCR1 = %d\n",htim16.Instance->CCR1);
		break;
	case _DEF_PWM4:
		//htim17.Instance->CCR1 = (uint32_t)pwm_tbl[ch].duty;
		//cliPrintf("htim17.Instance->CCR1 = %d\n",htim17.Instance->CCR1);
		break;
	}
}

uint16_t pwmRead(uint8_t ch)
{
	if (ch >= HW_PWM_MAX_CH) return 0;
	return pwm_tbl[ch].duty;
}

uint16_t pwmGetMax(uint8_t ch)
{
	if (ch >= HW_PWM_MAX_CH) return 255;
	return pwm_tbl[ch].max_value;
}

#ifdef _USE_HW_CLI
void cliPwm(cli_args_t *args)
{
	bool ret = true;
	uint8_t  ch;
	uint32_t pwm;

	uint8_t r, g, b;

	if (args->argc == 3)
	{
		ch  = (uint8_t)args->getData(1);
		pwm = (uint8_t)args->getData(2);

		ch = constrain(ch, 0, PWM_MAX_CH);

		if(args->isStr(0, "set"))
		{
			pwmWrite(ch, pwm);
			cliPrintf("pwm ch%d %d\n", ch, pwm);
		}
		else
		{
			ret = false;
		}
	}
	else if (args->argc == 2)
	{
		ch = (uint8_t)args->getData(1);

		if(args->isStr(0, "get"))
		{
			cliPrintf("pwm ch%d %d\n", ch, pwmRead(ch));
		}
		else
		{
			ret = false;
		}
	}

	else
	{
		ret = false;
	}

	if (ret == false)
	{
		cliPrintf( "args->argc:%d\n" , args->argc);
		cliPrintf( "pwm set 0~%d 0~255 \n", PWM_MAX_CH-1);
		cliPrintf( "pwm get 0~%d \n", PWM_MAX_CH-1);
	}
}
#endif

#if 0
//
//	NeoPixel Test
//
#define BIT_PERIOD      (207)
#define BIT_HIGH        (119)
#define BIT_LOW         (56)

//static uint8_t led_buf[50 + 24*64];

void NeoPixelDriver(uint32_t index, uint8_t red, uint8_t green, uint8_t blue){
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

	offset = 50;

	memcpy(&led_buf[offset + index*24 + 8*0], g_bit, 8*1);
	memcpy(&led_buf[offset + index*24 + 8*1], r_bit, 8*1);
	memcpy(&led_buf[offset + index*24 + 8*2], b_bit, 8*1);

	cliPrintf("NeoPixel out\n");
}
#endif

#endif

//
//	End file
//
