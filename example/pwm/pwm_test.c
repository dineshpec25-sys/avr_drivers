#include <stdint.h>
#include "../../include/pwm.h"
#include "../../include/timer.h"

int main(void)
{
    timer_init();
    pwm_init();

    while (1)
    {
        /* Tone 1: 500 Hz */
	for(uint8_t i = 0; i <= 9; i++)
	{pwm_tone(500);
        ms_delay(500);
        pwm_mute();
        ms_delay(300);}

        /* Tone 2: 800 Hz */
		for(uint8_t i = 0; i <= 9; i++){
        pwm_tone(800);
        ms_delay(500);
        pwm_mute();
        ms_delay(300);}

        /* Tone 3: 1200 Hz */
	for(uint8_t i = 0; i <= 9; i++)
	{
        pwm_tone(1200);
        ms_delay(500);
        pwm_mute();
        ms_delay(300);
	}

        /* Tone 4: 2000 Hz */
		for(uint8_t i = 0; i <= 9; i++)
		{        pwm_tone(2000);
        ms_delay(500);
        pwm_mute();
        ms_delay(3000);
		}
    }
}
