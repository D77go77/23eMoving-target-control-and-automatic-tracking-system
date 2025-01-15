#include "Led.h"


u8 LED_B,LED_G,LED_R=0;


void led_proc(void)
{
	// ledÖ»ÁÁ0.3s
	if (LED_B) {
		LED_B++;
		LED_B_ON;
	}
	if (LED_B > 3) {
		LED_B = 0;
		LED_B_OFF;
	}
	if (LED_G) {
		LED_G++;
		LED_G_ON;
	}
	if (LED_G > 3) {
		LED_G = 0;
		LED_G_OFF;
	}
	if (LED_R) {
		LED_R++;
		LED_R_ON;
	}
	if (LED_R > 3) {
		LED_R = 0;
		LED_R_OFF;
	}
}







