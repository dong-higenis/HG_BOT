
#include "can.h"

#define CAN_TX		3
#define CAN_RX		4

bool canInited = false;

void canSendLedState(uint8_t state) {
	CanFrame txFrame = { 0 };
  if(!canInited)
  {
    return;
  }
	txFrame.identifier = 0x500;
	txFrame.extd = 0;
	txFrame.data_length_code = 2;
	txFrame.data[0] = millis()%0xFF;
	txFrame.data[1] = state;
  ESP32Can.writeFrame(txFrame);
}

bool canInit()
{
  if(ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
      Serial.println("CAN bus started!");
      canInited = true;
  } else {
      Serial.println("CAN bus failed!");
      canInited = false;
  }
  return canInited;
}

bool isCanInit()
{
  return canInited;
}

bool canUpdate(CanFrame *rxFrame)
{  
  static uint32_t lastStamp = 0;
  if(!canInited)
  {
    return false;
  }
  if(millis() - lastStamp > 1000) { 
      lastStamp = millis();
      //sendObdFrame(5);
  }
  if(ESP32Can.readFrame(*rxFrame, 0)) {        
    return true;
  }
  return false;
}