#include "ap.h"


void apInit (void)
{
	cliOpen(HW_UART_CH_DEBUG, 115200);
	logBoot(true);

  ws2812Open(0,1);
  ws2812Open(1,1);

  servoOpen(0);
  servoOpen(1);

  canOpen(_DEF_CAN1, CAN_NORMAL, CAN_CLASSIC, CAN_500K, CAN_500K);
}

void apMain (void)
{
	uint32_t pre_time;

	pre_time = millis ();
	while (1)
	{
		if (millis () - pre_time >= 500)
		{
			pre_time = millis ();
			ledToggle (_DEF_LED1);
		}
		robotTask();
		cliMain();
	}
}




