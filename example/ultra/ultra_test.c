#include <stdint.h>

#include "../../include/gpio.h"
#include "../../include/timer.h"
#include "../../include/ultra.h"

int main(void)
{
    uint16_t distance;

    gpio_mode('E', 5, GPIO_OUTPUT);

    timer_init();
    ultra_init();

    while (1)
    {
        distance = ultra_get_distance_cm();

        if (distance > 0 && distance <= 15)
        {
            gpio_write('E', 5, HIGH);
        }
        else
        {
            gpio_write('E', 5, LOW);
        }

        ms_delay(100);
    }
}
