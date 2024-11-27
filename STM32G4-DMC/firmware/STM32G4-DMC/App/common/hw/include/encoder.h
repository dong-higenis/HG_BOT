#ifndef ENCODER_H_
#define ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ENCODER

#define ENCODER_MAX_CH  HW_ENCODER_MAX_CH


bool encoderInit(void);
bool encoderReset(void);
int32_t encoderGetSpeed(void);
uint32_t encoderGetPulseTime(void);
float encoderGetEstmatedSpeed(float time_ms);
int32_t encoderGetPosition(void);


#endif

#ifdef __cplusplus
}
#endif

#endif