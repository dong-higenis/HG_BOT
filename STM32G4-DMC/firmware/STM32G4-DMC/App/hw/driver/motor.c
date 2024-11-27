/*
 * motor.c
 *
 *  Created on: Nov 21, 2024
 *      Author: user
 */

#include "motor.h"

#ifdef _USE_HW_MOTOR
#include "pwm.h"
#include "qbuffer.h"
#include "cli.h"

#define NAME_DEF(x)  x, #x


typedef enum
{
  NONE  =  0,
  FRONT =  1,
  REAR  = -1
} MotorDirect_t;

typedef struct
{
  uint16_t pwm_offset;
  uint16_t pwm_max;
  uint16_t pwm_out[2];
  PwmPinName_t pin_name[2];
} motor_tbl_t;

motor_tbl_t motor_tbl[MOTOR_MAX_CH] = {
    {
        .pwm_offset = 0,
        .pwm_max = 255,
        .pwm_out = {0, 0},
        .pin_name = {mPWM_MT_L_IN1, mPWM_MT_L_IN2},
    },
    {
        .pwm_offset = 0,
        .pwm_max = 255,
        .pwm_out = {0, 0},
        .pin_name = {mPWM_MT_R_IN1, mPWM_MT_R_IN2},
    },
    {
        .pwm_offset = 0,
        .pwm_max = 255,
        .pwm_out = {0, 0},
        .pin_name = {mPWM_MT_FL_IN1, mPWM_MT_FL_IN2}
    },
    {
        .pwm_offset = 0,
        .pwm_max = 255,
        .pwm_out = {0, 0},
        .pin_name = {mPWM_MT_FR_IN1, mPWM_MT_FR_IN2}
    }
};


#if CLI_USE(HW_MOTOR)
static void cliMotor(cli_args_t *args);
#endif


bool motorInit(void)
{
#if CLI_USE(HW_MOTOR)
  cliAdd("motor", cliMotor);
#endif
  return true;
}


void motorWrite(int8_t ch, int16_t pwm_data, int8_t dir)
{
  pwm_data = constrain(pwm_data, 0, 100);

  if (pwm_data >= 0)
  {
    motor_tbl[ch].pwm_out[0] = cmap(pwm_data, 0, 100, motor_tbl[ch].pwm_offset, motor_tbl[ch].pwm_max);
    motor_tbl[ch].pwm_out[1] = 0;
  }
  else if (pwm_data < 0)
  {
    pwm_data = -pwm_data;
    motor_tbl[ch].pwm_out[0] = 0;
    motor_tbl[ch].pwm_out[1] = cmap(pwm_data, 0, 100, motor_tbl[ch].pwm_offset, motor_tbl[ch].pwm_max);
  }

  if (dir == FRONT)
  {
    pwmWrite(motor_tbl[ch].pin_name[0], motor_tbl[ch].pwm_out[0]);
    pwmWrite(motor_tbl[ch].pin_name[1], motor_tbl[ch].pwm_out[1]);
  }
  else
  {
    pwmWrite(motor_tbl[ch].pin_name[0], motor_tbl[ch].pwm_out[1]);
    pwmWrite(motor_tbl[ch].pin_name[1], motor_tbl[ch].pwm_out[0]);
  }
}


#if CLI_USE(HW_MOTOR)
void cliMotor(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 4 && args->isStr(0, "move"))
  {
    int8_t ch;
    int8_t speed;
    int8_t dir;

    ch    = (int8_t)args->getData(1);
    speed = (int8_t)args->getData(2);
    dir   = (int8_t)args->getData(3);

    motorWrite(ch, speed, dir);

    while(cliKeepLoop())
    {
      cliPrintf("motor move ch[%d] speed[%d] dir[%d]\n", ch, speed, dir);
      cliMoveUp(1);
      delay(10);
    }
    cliMoveDown(1);

    motorWrite(ch, 0, dir);

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("motor info\n");
    cliPrintf("motor move ch[0~3] speed[0~100] dir[front:1,rear:-1]\n");
  }

}
#endif

#endif
