/*
 * servo.c
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */
#include "servo.h"
#include "cli.h"
#include <math.h>


#ifdef _USE_HW_SERVO

typedef struct
{
	uint32_t servo_duty;
	int32_t now_servo_angle;
	int16_t step_angle;
	int16_t step_val;
	int16_t target_val;
	int16_t move_count;
	
	bool action_command;
	bool start_move_flag;
} servo_info_t;

#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args);
#endif

servo_info_t servo_info[HW_SERVO_MAX_CH];

bool servo_init = false;

extern TIM_HandleTypeDef htim2;	//	NeoPixel 0
extern TIM_HandleTypeDef htim3;	//	NeoPixel 1

bool servoInit(void)
{
	servo_init = true;

#ifdef _USE_HW_CLI
	cliAdd("servo", cliServo);
#endif
	return true;
}

void servoOpen(uint32_t ch)
{
  switch(ch)
  {
  case 0:
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    break;

  case 1:
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    break;
  }
  servoSetPos(ch, 0);
}

int16_t servoGetPos(uint8_t ch)
{
	if(ch >= HW_SERVO_MAX_CH)
	{
		return 0;
	}
	return servo_info[ch].now_servo_angle;
}

void servoSetPos(uint8_t ch, int16_t angle)
{
	if(ch >= HW_SERVO_MAX_CH)
	{
		return;
	}
	if(angle > 90 || angle < -90)
	{
		cliPrintf("Out of Range [%d]\n", angle);
		return;
	}
	servo_info[ch].now_servo_angle = angle;
	//float temp_servo_duty_per = (5.0 + ((float)angle * 0.0278));	//	1ms~2ms version
	float temp_servo_duty_per = (2.5 + ((float)(angle + 90)* 0.0555556));	//	0.5ms~2.5ms version
	float temp_servo_duty = ( temp_servo_duty_per  / 100.0 ) * 10000.0;
	servo_info[ch].servo_duty = (uint32_t)temp_servo_duty;

	//cliPrintf("target angle = %d\n", (int)angle);
	//cliPrintf("temp_servo_duty_per = %d\n", (int)temp_servo_duty_per);
	//cliPrintf("temp_servo_duty     = %lf\n", temp_servo_duty);

	switch(ch)
	{
	case _DEF_PWM1:
		htim3.Instance->CCR2 = servo_info[ch].servo_duty;		
		break;
	case _DEF_PWM2:
		htim2.Instance->CCR1 = servo_info[ch].servo_duty;		
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

#ifdef _USE_HW_CLI
void cliServo(cli_args_t *args)
{
	bool ret = false;
	uint8_t	ch;
	int32_t angle;
	

	if (args->argc == 1 && args->isStr(0, "info") == true)
	{		
		for(uint8_t i=0;i<HW_SERVO_MAX_CH;i++)
		{
			cliPrintf("%d - %d'(duty: %d)\n", i, servo_info[i].now_servo_angle, servo_info[i].servo_duty);
		}
		ret = true;
	}

	if (args->argc == 4 && args->isStr(0, "set") == true)
	{
		if(args->isStr(1, "pos") == true)
		{
			ch  = (uint8_t)args->getData(2);
			angle = (int32_t)args->getData(3);
		
			cliPrintf("set pos ch=%d, angle=%d\n", ch, angle);
			servoSetPos(ch, angle);
			ret = true;
		}
	}

	if (args->argc == 3 && args->isStr(0, "get") == true)
	{
		if(args->isStr(1, "pos") == true)
		{
			ch  = (uint8_t)args->getData(2);
			if(ch < HW_SERVO_MAX_CH)
			{				
				cliPrintf("pos ch=%d, angle=%d\n", ch, servo_info[ch].now_servo_angle);						
				ret = true;
			}
		}
	}

	if (ret == false)
	{
	  cliPrintf("servo info\n");
		cliPrintf("servo set pos [ch] [angle]\n");
		cliPrintf("servo get pos [ch]\n");
		cliPrintf("servo continue [ch] [speed] [angle]\n");
	}
}
#endif

#endif
