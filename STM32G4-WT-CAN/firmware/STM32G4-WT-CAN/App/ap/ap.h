#ifndef AP_H_
#define AP_H_


#include "ap_def.h"

#define CAN_ID_WT_WEIGHT            (0x120)
#define CAN_ID_COMMANDER_WT_CAL_SET (0x202)



void apInit(void);
void apMain(void);


#endif
