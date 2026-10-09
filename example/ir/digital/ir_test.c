#include "../../../include/gpio.h"
#include "../../../include/ir.h"

int main(void)
{
    gpio_mode('E', 5, GPIO_OUTPUT);
    ir_init();

    while (1)
    {
        if (ir_get_status() == OCCUPIED)
            gpio_write('E', 5, HIGH);
        else
            gpio_write('E', 5, LOW);
    }
}
