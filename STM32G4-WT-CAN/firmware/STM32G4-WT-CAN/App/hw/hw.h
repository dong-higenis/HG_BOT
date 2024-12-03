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
#include "cli.h"
#include "util.h"
#include "swtimer.h"
#include "button.h"
#include "rtc.h"
#include "cdc.h"
#include "can.h"
#include "i2c.h"
#include "gpio.h"
#include "fram.h"
#include "imu.h"
#include "hx711.h"
#include "flash.h"

bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif
