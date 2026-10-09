#include <stdint.h>

#include "../../include/gpio.h"
#include "../../include/timer.h"
/* LCD mapped to Port A */
#define LCD_PORT 'A'

#define LCD_RS 0
#define LCD_EN 1

#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 4
#define LCD_D7 5

static void lcd_write_nibble(uint8_t nibble)
{
    gpio_write(LCD_PORT, LCD_D4, (nibble & 0x01) ? HIGH : LOW);
    gpio_write(LCD_PORT, LCD_D5, (nibble & 0x02) ? HIGH : LOW);
    gpio_write(LCD_PORT, LCD_D6, (nibble & 0x04) ? HIGH : LOW);
    gpio_write(LCD_PORT, LCD_D7, (nibble & 0x08) ? HIGH : LOW);
}

static void lcd_enable_pulse(void)
{
    gpio_write(LCD_PORT, LCD_EN, HIGH);
    ms_delay(1);

    gpio_write(LCD_PORT, LCD_EN, LOW);
    ms_delay(1);
}

static void lcd_send_nibble(uint8_t nibble)
{
    lcd_write_nibble(nibble);
    lcd_enable_pulse();
}

static void lcd_send_byte(uint8_t value)
{
    lcd_send_nibble(value >> 4);
    lcd_send_nibble(value & 0x0F);
}

/* ---------- LCD PUBLIC FUNCTIONS ---------- */

static void lcd_command(uint8_t command)
{
    gpio_write(LCD_PORT, LCD_RS, LOW);
    lcd_send_byte(command);

    if (command == 0x01 || command == 0x02)
        ms_delay(2);
}

static void lcd_data(uint8_t data)
{
    gpio_write(LCD_PORT, LCD_RS, HIGH);
    lcd_send_byte(data);
}

static void lcd_clear(void)
{
    lcd_command(0x01);
}

static void lcd_set_cursor(uint8_t row, uint8_t col)
{
    if (row > 1) row = 1;
    if (col > 15) col = 15;

    lcd_command(0x80 | ((row == 0) ? col : (0x40 + col)));
}

static void lcd_string(const char *str)
{
    while (*str != '\0')
        lcd_data((uint8_t)*str++);
}

static void lcd_init(void)
{
    gpio_mode(LCD_PORT, LCD_RS, GPIO_OUTPUT);
    gpio_mode(LCD_PORT, LCD_EN, GPIO_OUTPUT);
    gpio_mode(LCD_PORT, LCD_D4, GPIO_OUTPUT);
    gpio_mode(LCD_PORT, LCD_D5, GPIO_OUTPUT);
    gpio_mode(LCD_PORT, LCD_D6, GPIO_OUTPUT);
    gpio_mode(LCD_PORT, LCD_D7, GPIO_OUTPUT);

    gpio_write(LCD_PORT, LCD_RS, LOW);
    gpio_write(LCD_PORT, LCD_EN, LOW);

    ms_delay(40);

    /* HD44780 initialization sequence */
    lcd_send_nibble(0x03);
    ms_delay(5);

    lcd_send_nibble(0x03);
    ms_delay(2);

    lcd_send_nibble(0x03);
    ms_delay(2);

    lcd_send_nibble(0x02);
    ms_delay(2);

    lcd_command(0x28);  /* 4-bit, 2-line mode */
    lcd_command(0x0C);  /* Display ON, cursor OFF */
    lcd_command(0x06);  /* Increment cursor */
    lcd_clear();
}

/* ---------- MAIN ---------- */

int main(void)
{
    timer_init();
    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_string("LCD TEST");

    lcd_set_cursor(1, 0);
    lcd_string("ATmega2560");

    while (1)
    {
        /* Keep displaying the test message */
    }
}
