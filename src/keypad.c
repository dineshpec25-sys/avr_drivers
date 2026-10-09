#include "../include/keypad.h"
#include "../include/gpio.h"
#include "../hm/keypad_regs.h"

static const char keypad_matrix[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void keypad_init(void)
{
    gpio_mode(KEYPAD_PORT, KEYPAD_R0, GPIO_OUTPUT);
    gpio_mode(KEYPAD_PORT, KEYPAD_R1, GPIO_OUTPUT);
    gpio_mode(KEYPAD_PORT, KEYPAD_R2, GPIO_OUTPUT);
    gpio_mode(KEYPAD_PORT, KEYPAD_R3, GPIO_OUTPUT);

    gpio_mode(KEYPAD_PORT, KEYPAD_C0, GPIO_INPUT_PULLUP);
    gpio_mode(KEYPAD_PORT, KEYPAD_C1, GPIO_INPUT_PULLUP);
    gpio_mode(KEYPAD_PORT, KEYPAD_C2, GPIO_INPUT_PULLUP);
    gpio_mode(KEYPAD_PORT, KEYPAD_C3, GPIO_INPUT_PULLUP);

    gpio_write(KEYPAD_PORT, KEYPAD_R0, HIGH);
    gpio_write(KEYPAD_PORT, KEYPAD_R1, HIGH);
    gpio_write(KEYPAD_PORT, KEYPAD_R2, HIGH);
    gpio_write(KEYPAD_PORT, KEYPAD_R3, HIGH);
}

char keypad_get_key(void)
{
    uint8_t r, c;
    uint8_t row_pins[4] = {KEYPAD_R0, KEYPAD_R1, KEYPAD_R2, KEYPAD_R3};
    uint8_t col_pins[4] = {KEYPAD_C0, KEYPAD_C1, KEYPAD_C2, KEYPAD_C3};

    for (r = 0; r < 4; r++)
    {
        gpio_write(KEYPAD_PORT, row_pins[r], LOW);

        for (c = 0; c < 4; c++)
        {
            if (gpio_read(KEYPAD_PORT, col_pins[c]) == LOW)
            {
                while (gpio_read(KEYPAD_PORT, col_pins[c]) == LOW);
                
                gpio_write(KEYPAD_PORT, row_pins[r], HIGH);
                
                return keypad_matrix[r][c];
            }
        }

        gpio_write(KEYPAD_PORT, row_pins[r], HIGH);
    }

    return '\0';
}
