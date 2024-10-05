#include "ap.h"

//void updateCan(void);


void apInit (void)
{
	cliOpen(HW_UART_CH_DEBUG, 115200);
	logBoot(true);
	canOpen(_DEF_CAN1, CAN_NORMAL, CAN_CLASSIC, CAN_500K, CAN_500K);
}

void apMain (void)
{
	uint32_t pre_time;
	uint32_t led_case = 0;

	pre_time = millis ();
	while (1)
	{
		if (millis () - pre_time >= 500)
		{
			led_case++;
			if(led_case >= 3){
				led_case = 0;
			}
			pre_time = millis ();
			ledToggle (_DEF_LED1);
		}

		cliMain();
	}
}
#if 0
void updateCan(void)
{	
	imu_info_t imu_info;
	short r, p, y;
    static uint8_t data_count = 0;
    static uint32_t pre_time = 0;
	uint8_t dummy = 0; //status

    can_msg_t msg;
	if(millis() - pre_time > 100)
	{
		pre_time = millis();
		if(imuGetInfo(&imu_info))
		{
			data_count++;
        	r = (short)imu_info.roll * 1;
        	p = (short)imu_info.pitch * 1;
        	y = (short)imu_info.yaw * 1;
			//cliPrintf("%d\t Roll: %d\t Pitch: %d\t Yaw: %d\n ", data_count, r, p, y);
				
			msg.frame   = CAN_CLASSIC;
			msg.id_type = CAN_EXT;
			msg.dlc     = CAN_DLC_8;
			msg.id      = CAN_ID_IMU_AXIS;
			msg.length  = CAN_DLC_8;
			msg.data[0] = data_count;
			msg.data[1] = (buttonGetPressed(0) & 0x0F) | ((dummy << 4) & 0xF0);
			msg.data[2] = (r >> 16);
			msg.data[3] = (r & 0xFF);
			msg.data[4] = (p >> 16);
			msg.data[5] = (p & 0xFF);
			msg.data[6] = (y >> 16);
			msg.data[7] = (y & 0xFF);
			
			if (canMsgWrite(_DEF_CAN1, &msg, 10) > 0)
			{

			}		
		}
	} 
}
#endif
