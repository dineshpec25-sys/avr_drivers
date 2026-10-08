#include "../../include/pwm.h"
#include "../../include/timer.h"

int main(void)
{
    pwm_init();
    timer_init();

    pwm_start();

    while (1)
    {
        pwm_set_duty(25);
        ms_delay(3000);

        pwm_set_duty(50);
        ms_delay(3000);

        pwm_set_duty(75);
        ms_delay(3000);

        pwm_set_duty(100);
        ms_delay(3000);
    }
}
