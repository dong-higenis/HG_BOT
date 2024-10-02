/*
 * servo.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "servo.h"
#include "cli.h"
#include <math.h>

#define ANGLE_0		750
#define ANGLE_M90	250
#define ANGLE_90	1250

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
static servo_pwm_tbl_t servo_pwm_tbl[2];

bool servo_init = false;

extern TIM_HandleTypeDef htim2;	//	NeoPixel 0
extern TIM_HandleTypeDef htim3;	//	NeoPixel 1

//	CLI function
#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args);
#endif

bool servoInit(void)
{
	//servo_pwm_tbl[0].max_value = 32000;
	//servo_pwm_tbl[1].max_value = 32000;

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

void servoSet(uint32_t ch, uint8_t speed, int16_t angle)
{
	//float temp_servo_duty_per = (5.0 + ((float)angle * 0.0278));	//	1ms~2ms version
	float temp_servo_duty_per = (2.5 + ((float)(angle + 90)* 0.0555556));	//	0.5ms~2.5ms version
	float temp_servo_duty = ( temp_servo_duty_per  / 100.0 ) * 10000.0;
	uint32_t servo_duty = (uint32_t)temp_servo_duty;

	cliPrintf("temp_servo_duty_per = %d\n", (int)temp_servo_duty_per);
	cliPrintf("temp_servo_duty     = %lf\n", temp_servo_duty);

	switch(ch)
	{
	case _DEF_PWM1:
		htim3.Instance->CCR2 = servo_duty;
		cliPrintf("htim3.Instance->CCR2 = %d\n",htim3.Instance->CCR2);
		break;
	case _DEF_PWM2:
		htim2.Instance->CCR1 = servo_duty;
		cliPrintf("htim2.Instance->CCR1 = %d\n",htim2.Instance->CCR1);
		break;
	}
}

uint32_t calAngleToRegval(int16_t angle)
{
	float temp_servo_duty_per = (2.5 + ((float)(angle + 90) * 0.0555556));	//	0.5ms~2.5ms version
	float temp_servo_duty = ( temp_servo_duty_per  / 100.0 ) * 10000.0;
	return (uint32_t)temp_servo_duty;
}

int16_t calRegvalToAngle(uint32_t register_data)
{
    float temp_servo_duty_per = ((float)register_data / 10000.0) * 100.0;
    int16_t angle = (int16_t)((temp_servo_duty_per - 2.5) / 0.0555556) - 90;
    return angle;
}

//	servoSetContinue function control global flag
bool action_0ch_command = false;
bool action_1ch_command = false;

bool start_move_0ch_flag = false;
bool start_move_1ch_flag = false;

// 	servoSetContinue function variable
//	Now Angle
int32_t now_0ch_servo_angle = 0;
int32_t now_1ch_servo_angle = 0;
int16_t step_0ch_angle = 0;
int16_t step_1ch_angle = 0;

int16_t step_0ch_val = 0;
int16_t step_1ch_val = 0;
int16_t target_0ch_val = 0;
int16_t target_1ch_val = 0;

int16_t move_0ch_count=0;
int16_t move_1ch_count=0;

//	Main Thread Operation task
void servoSetContinue(void)
{
	//	get now angle
	now_0ch_servo_angle = calRegvalToAngle(htim3.Instance->CCR2);
	now_1ch_servo_angle = calRegvalToAngle(htim2.Instance->CCR1);

	//	control
	if(action_0ch_command == true)
	{
		//	get target reg val
		target_0ch_val = calAngleToRegval(target_0ch_val);
		//	now ccr get
		now_0ch_servo_angle = htim3.Instance->CCR2;
		//	comparison now vs target
		if(target_0ch_val != now_0ch_servo_angle)
		{
			//	start up
			if(start_move_0ch_flag == true)
			{
				step_0ch_val = (int16_t)((float)(target_0ch_val - now_0ch_servo_angle) / (float)step_0ch_angle);
				start_move_0ch_flag = false;
			}
			// move continue
			else
			{
				//step_0ch_val
				now_0ch_servo_angle = now_0ch_servo_angle + step_0ch_val;
				//	서보 모터 오버런 방지
				//	0' = 750
				if(target_0ch_val == ANGLE_0)
				{
					if(abs(step_0ch_val) > abs(target_0ch_val - now_0ch_servo_angle))
					{
						now_0ch_servo_angle = ANGLE_0;
					}
				}
				// -90' = 250
				if((now_0ch_servo_angle >= ANGLE_M90)&&(now_0ch_servo_angle < ANGLE_0))
				{
					//	step 편차로 인한 타겟 오버런
					if(now_0ch_servo_angle < target_0ch_val )
					{
						now_0ch_servo_angle = target_0ch_val;
					}
				}
				else if(now_0ch_servo_angle < ANGLE_M90)
				{
					now_0ch_servo_angle = ANGLE_M90;
				}
				// 90' = 1250
				if((now_0ch_servo_angle <= ANGLE_90) && (now_0ch_servo_angle > ANGLE_0))
				{
					//	step 편차로 인한 타겟 오버런
					if(now_0ch_servo_angle > target_0ch_val )
					{
						now_0ch_servo_angle = target_0ch_val;
					}
				}
				else if(now_0ch_servo_angle > ANGLE_90)
				{
					now_0ch_servo_angle = ANGLE_90;
				}
			}

			//now_0ch_servo_angle
			now_0ch_servo_angle = calRegvalToAngle(now_0ch_servo_angle);
			servoSet(0,0,now_0ch_servo_angle);
		}
		else
		{
			action_0ch_command = false;
		}

	}
}

#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args)
{
	int i;
	bool ret = true;
	uint8_t	ch;
	uint8_t speed;
	uint32_t angle;
	int32_t set_angle;

	//	servo <ch> <speed> <angle>
	cliPrintf("args->argc = %d\n",args->argc);
	if (args->argc == 3)
	{
		ch  = (uint8_t)args->getData(0);
		speed = (uint8_t)args->getData(1);
		angle = (uint32_t)args->getData(2);

		//
		cliPrintf("ch=%d, speed=%d, angle=%d\n", ch, speed, angle);
		servoSet(ch, speed, angle);
	}
	//	servo continue <ch> <speed> <angle>
	if (args->argc == 4)
	{
		ch  = (uint8_t)args->getData(1);
		speed = (uint8_t)args->getData(2);
		set_angle = (int32_t)args->getData(3);

		if(args->isStr(0, "continue"))
		{
			if(ch == 0)
			{
				step_0ch_val = speed;
				target_0ch_val = set_angle;
				action_0ch_command = true;
				start_move_0ch_flag = true;
				step_0ch_angle = calAngleToRegval(speed);
			}
			else if(ch == 1)
			{
				step_1ch_val = speed;
				target_1ch_val = set_angle;
				action_1ch_command = true;
				start_move_1ch_flag = true;
				step_1ch_angle = calAngleToRegval(speed);
			}
		}
	}
	//	servo set <value>
	else if(args->argc == 2)
	{
		angle = (uint32_t)args->getData(1);
		if(args->isStr(0, "set"))
		{
			htim3.Instance->CCR2 = angle;
			htim2.Instance->CCR1 = angle;
		}
	}
	//	servo now
	else if(args->argc == 1)
	{
		if(args->isStr(0, "now"))
		{
			cliPrintf("0:%d'\n",now_0ch_servo_angle);
			cliPrintf("1:%d'\n",now_1ch_servo_angle);
		}
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
