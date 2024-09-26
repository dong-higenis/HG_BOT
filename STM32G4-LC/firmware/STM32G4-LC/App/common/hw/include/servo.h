/*
 * servo.h
 *
 *  Created on: Sep 26, 2024
 *      Author: user
 */

#ifndef COMMON_HW_INCLUDE_SERVO_H_
#define COMMON_HW_INCLUDE_SERVO_H_

#include "hw_def.h"

bool servoInit(void);
void servoBegin(void);
void servoSet(uint32_t ch, uint8_t speed, uint8_t angle);

#endif /* COMMON_HW_INCLUDE_SERVO_H_ */
