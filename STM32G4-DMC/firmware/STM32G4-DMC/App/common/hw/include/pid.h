/*
 * pid.h
 *
 *  Created on: Nov 20, 2024
 *      Author: user
 */

#ifndef COMMON_HW_INCLUDE_PID_H_
#define COMMON_HW_INCLUDE_PID_H_

#include "hw_def.h"


#ifdef _USE_HW_PID


typedef struct
{
  float kp;
  float ki;
  float kd;

  float p_err;
  float i_err;
  float d_err;

  float target;
  float error;
  float error_pre;
  float error_sum;
  float error_dif;

  float input;
  float output;

  float out_max;
  float out_min;

  float sum_max;
  float sum_min;

} pid_cfg_t;


bool  pidInit(pid_cfg_t *p_pid);
void  pidSetGain(pid_cfg_t *p_pid, float kp, float ki, float kd);
void  pidCompute(pid_cfg_t *p_pid);
float pidGetOut(pid_cfg_t *p_pid);
void  pidClearErr(pid_cfg_t *p_pid);

#endif

#endif /* COMMON_HW_INCLUDE_PID_H_ */
