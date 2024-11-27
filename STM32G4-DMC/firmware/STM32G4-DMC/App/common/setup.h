/*
 * setup.h
 *
 *  Created on: Nov 20, 2024
 *      Author: user
 */

#ifndef COMMON_SETUP_H_
#define COMMON_SETUP_H_

//-- H/W
//
#define HW_GEAR_RATIO               336.0f  // 기어비
#define HW_ENCODER_PPR               24.0f  // pps 엔코더 1회전시 펄스수
#define HW_MM_1ROUND                  8.0f  // mm  축 1회전시 가는 거리
#define HW_PULSE_1ROUND    (HW_GEAR_RATIO * HW_ENCODER_PPR) // 축 1회전시 펄스 수
#define HW_PID_FREQ                    100  // hz  pid 제어 주기
#define HW_SPEED_MAX_RPM   _CFG_MOTOR_MAX_RPM
#define HW_LIMIT_ERROR                 300  // 리미트 판단 위치 에러 기준값(ms)
#define HW_LIMIT_ERROR_MOVE           0.5f  // 리미트 걸렸을때 뒤로 이동하는 값(mm)
#define HW_MOTOR_DIR                     1  // 모터 방향 1, -1

#define HW_PID_POS_P_GAIN               20
#define HW_PID_POS_I_GAIN                0
#define HW_PID_POS_D_GAIN                0

#define HW_PID_SPD_P_GAIN               50  // 50
#define HW_PID_SPD_I_GAIN               10  // 10
#define HW_PID_SPD_D_GAIN                0  //  0

#endif /* COMMON_SETUP_H_ */
