#include "ap.h"



void apInit (void)
{
	cliOpen(HW_UART_CH_DEBUG, 115200);
	logBoot(true);
	ws2812Begin(2);
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
		imuUpdate();
		cliMain();
	}
}


