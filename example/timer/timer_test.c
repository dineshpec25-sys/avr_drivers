#include "../../include/gpio.h"
#include "../../include/timer.h"

int main(void)
{
    gpio_mode('E', 5, GPIO_OUTPUT);

    timer_init();

    while (1)
    {
        gpio_write('E', 5, HIGH);
        ms_delay(1000);

        gpio_write('E', 5, LOW);
        ms_delay(1000);
    }
}
