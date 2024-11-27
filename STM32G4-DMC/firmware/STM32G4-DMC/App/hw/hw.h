#ifndef HW_H_
#define HW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#include "led.h"
#include "uart.h"
#include "qbuffer.h"
#include "log.h"
#include "gpio.h"
#include "cli.h"
#include "util.h"
#include "swtimer.h"
#include "button.h"
//#include "rtc.h"
//#include "usb.h"
#include "cdc.h"
#include "can.h"
#include "i2c.h"
#include "fram.h"

//#include "ws2812.h"
//#include "servo.h"

#include "pwm.h"
#include "motor.h"
#include "encoder.h"


bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif
