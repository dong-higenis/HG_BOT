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

			switch(led_case){
			case 0:
				ws2812SetColor(0, 255, 0, 0);
				break;
			case 1:
				ws2812SetColor(0, 0, 255, 0);
				break;
			case 2:
				ws2812SetColor(0, 0, 0, 255);
				break;
			}
		}
		cliMain();
	}
}


