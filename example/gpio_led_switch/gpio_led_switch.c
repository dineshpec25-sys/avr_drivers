#include "../../include/gpio.h"

int main(void)
{
    gpio_mode('E', 5, GPIO_OUTPUT);
    gpio_mode('H', 6, GPIO_INPUT_PULLUP);

    while (1)
    {
        if (gpio_read('H', 6) == LOW)
            gpio_write('E', 5, HIGH);
        else
            gpio_write('E', 5, LOW);
    }
}
