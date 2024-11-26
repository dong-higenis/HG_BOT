#ifndef _CAN_H
#define _CAN_H

#include <ESP32-TWAI-CAN.hpp>

bool canInit();
bool canUpdate(CanFrame *rxFrame);
bool isCanInit();
void canSendLedState(uint8_t state);

#endif