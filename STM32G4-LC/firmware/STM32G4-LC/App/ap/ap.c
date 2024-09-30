#include "ap.h"

void apInit (void)
{
	cliOpen(HW_UART_CH_DEBUG, 115200);
	logBoot(true);
	ws2812Begin(1);
	servoBegin();
}

void apMain (void)
{
	uint32_t pre_time;
	uint32_t servo_time;
	//uint32_t led_case = 0;

	pre_time = millis ();
	servo_time = millis ();
	while (1)
	{
		if (millis () - pre_time >= 500)
		{
			pre_time = millis ();
			ledToggle (_DEF_LED1);

#if 0
			led_case++;
			if(led_case >= 3){
				led_case = 0;
			}

			//	Example NeoPixel Code , 500ms
			switch(led_case){
			case 0:
				ws2812SetColor(0, 0, 255, 0, 0);
				ws2812SetColor(0, 1, 255, 0, 0);
				break;
			case 1:
				ws2812SetColor(0, 0, 0, 255, 0);
				ws2812SetColor(0, 1, 0, 255, 0);
				break;
			case 2:
				ws2812SetColor(0, 0, 0, 0, 255);
				ws2812SetColor(0, 1, 0, 0, 255);
				break;
			}
#endif
		}

		//	servo motor continue
		if (millis () - servo_time >= 500)
		{
			servo_time = millis ();
			servoSetContinue();
		}

		cliMain();
	}
}


