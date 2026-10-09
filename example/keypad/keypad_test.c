#include <stdint.h>

#include "../../include/gpio.h"
#include "../../include/keypad.h"

/*
 * BCD output:
 * D9 = Bit 3 (MSB)
 * D8 = Bit 2
 * D7 = Bit 1
 * D6 = Bit 0 (LSB)
 */
static void bcd_display(uint8_t value)
{
    gpio_write('H', 6, (value & 0x08) ? HIGH : LOW);
    gpio_write('H', 5, (value & 0x04) ? HIGH : LOW);
    gpio_write('H', 4, (value & 0x02) ? HIGH : LOW);
    gpio_write('H', 3, (value & 0x01) ? HIGH : LOW);
}

int main(void)
{
    char key;

    /* Configure BCD LED outputs */
    gpio_mode('H', 6, GPIO_OUTPUT);  // D9
    gpio_mode('H', 5, GPIO_OUTPUT);  // D8
    gpio_mode('H', 4, GPIO_OUTPUT);  // D7
    gpio_mode('H', 3, GPIO_OUTPUT);  // D6

    /* Initialize the keypad */
    keypad_init();

    /* Initially display 0000 */
    bcd_display(0);

    while (1)
    {
        key = keypad_get_key();

        /* Accept numeric keys only */
        if (key >= '0' && key <= '9')
        {
            uint8_t digit = (uint8_t)(key - '0');

            bcd_display(digit);
        }
    }
}
