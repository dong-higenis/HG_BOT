/*
 * robot.c
 *
 *  Created on: Oct 7, 2024
 *      Author: user
 */
#include "ap_def.h"
#include "robot.h"

#define CAN_ID_LC_STATUS 0x110
#define CAN_ID_LED_SET   (0x200)
#define CAN_ID_SERVO_SET (0x201)

const uint32_t can_cmd_list[] = {
  CAN_ID_LED_SET,
  CAN_ID_SERVO_SET
};
#define MAX_CAN_CMD_LIST (sizeof(can_cmd_list)/sizeof(uint32_t))

void robotTask(void)
{
  updateCan();
  //  update device
}

void updateCan(void)
{
  static uint8_t data_count = 0;
  static uint32_t pre_time = 0;
  uint8_t dummy = 0; //status

  can_msg_t msg;
  if(millis() - pre_time > 100)
  {
    pre_time = millis();
    data_count++;

    msg.frame   = CAN_CLASSIC;
    msg.id_type = CAN_EXT;
    msg.dlc     = CAN_DLC_8;
    msg.id      = CAN_ID_LC_STATUS;
    msg.length  = CAN_DLC_8;
    msg.data[0] = data_count;
    msg.data[1] = (buttonGetPressed(0) & 0x0F) | ((dummy << 4) & 0xF0);
    msg.data[2] = (servoGetPos(0)>>8);
    msg.data[3] = (servoGetPos(0) & 0xFF);
    msg.data[4] = (servoGetPos(1)>>8);
    msg.data[5] = (servoGetPos(1) & 0xFF);
    msg.data[6] = 0;
    msg.data[7] = 0;

    if (canMsgWrite(_DEF_CAN1, &msg, 10) > 0)
    {

    }
  }
  if (canMsgAvailable(_DEF_CAN1))
  {
    canMsgRead(_DEF_CAN1, &msg);
    for(uint8_t i=0;i<MAX_CAN_CMD_LIST;i++)
    {
      if(msg.id == can_cmd_list[i])
      {
        cliPrintf("0x%08X, L:%02d, ", msg.id, msg.length);
        for (int i=0; i<msg.length; i++)
        {
          cliPrintf("0x%02X ", msg.data[i]);
        }
        cliPrintf("\n");

        switch(msg.id)
        {
        case CAN_ID_LED_SET:
          uint8_t ch = msg.data[0];
          uint8_t idx = msg.data[1];
          uint8_t r = msg.data[2];
          uint8_t g = msg.data[3];
          uint8_t b = msg.data[4];
          ws2812SetColor(ch, idx, r, g, b);
          break;
        case CAN_ID_SERVO_SET:
        {
          uint8_t ch = msg.data[0];
          int16_t pos = (msg.data[1] << 8) | msg.data[2];
          //uint16_t speed = (msg.data[3] << 8) | msg.data[4];
          cliPrintf("ch:%d pos:%d\n",ch,pos);
          cliPrintf("msg.data[1]:%d msg.data[2]:%d\n",msg.data[1],msg.data[2]);
          servoSetPos(ch, pos);
        }
          break;
        }
      }
    }
  }
}
