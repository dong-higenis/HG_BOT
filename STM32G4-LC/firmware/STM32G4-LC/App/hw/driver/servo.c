/*
 * servo.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "servo.h"
#include "cli.h"

//
//	PWM driver struct
//
typedef struct
{
	uint16_t max_value;
	uint16_t duty;
	uint32_t channel;
	uint32_t step;
} servo_pwm_tbl_t;
static servo_pwm_tbl_t servo_pwm_tbl[PWM_MAX_CH];

bool servo_init = false;

//	CLI function
#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args);
#endif

bool servoInit(void)
{

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

#ifdef _USE_HW_CLI
	cliAdd("servo", cliServo);
#endif
	servo_init = true;
	return true;
}

void servoBegin(void)
{


}

void servoSet(uint32_t ch, uint8_t speed, uint8_t angle)
{

}

#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args)
{
	bool ret = true;
	uint8_t	ch;
	uint8_t speed;
	uint32_t angle;

	//	servo <ch> <speed> <angle>
	cliPrintf("args->argc = %d\n",args->argc);
	if (args->argc == 3)
	{
		ch  = (uint8_t)args->getData(0);
		speed = (uint8_t)args->getData(1);
		angle = (uint32_t)args->getData(2);

		//	data limit

		//
		cliPrintf("ch=%d, speed=%d, angle=%d\n", ch, speed, angle);
		//ws2812SetColor(ch, led_number, r, g, b);
	}
	else
	{
		ret = false;
	}

	if (ret == false)
	{
		cliPrintf("Command format => servo <ch> <speed> <angle>\n");

	}
}
#endif
