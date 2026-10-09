 
#include <stdint.h>
#include "../include/gpio.h"
#include "../include/timer.h"
#include "../include/lcd.h"

/* Generate a short Enable pulse */
static void lcd_enable_pulse(void)
{
    gpio_write(LCD_PORT, LCD_EN, HIGH);

    for (volatile uint16_t i = 0; i < 100; i++)
    {
        /* Short pulse delay */
    }

    gpio_write(LCD_PORT, LCD_EN, LOW);

    for (volatile uint16_t i = 0; i < 100; i++)
    {
        /* Short settling delay */
    }
}

/* Send 4 bits to LCD */
static void lcd_write_nibble(uint8_t value)
{
    gpio_write(LCD_PORT, LCD_D4,
               (value & 0x01) ? HIGH : LOW);

    gpio_write(LCD_PORT, LCD_D5,
               (value & 0x02) ? HIGH : LOW);

    gpio_write(LCD_PORT, LCD_D6,
               (value & 0x04) ? HIGH : LOW);

    gpio_write(LCD_PORT, LCD_D7,
               (value & 0x08) ? HIGH : LOW);

    lcd_enable_pulse();
}

/* Send one byte in 4-bit mode */
static void lcd_write_byte(uint8_t value)
{
    lcd_write_nibble(value >> 4);
    lcd_write_nibble(value & 0x0F);
}

/* Send LCD instruction */
void lcd_command(uint8_t command)
{
    gpio_write(LCD_PORT, LCD_RS, LOW);
    lcd_write_byte(command);

    if (command == 0x01 || command == 0x02)
    {
        ms_delay(2);
    }
}

/* Send one character */
void lcd_data(uint8_t data)
{
    gpio_write(LCD_PORT, LCD_RS, HIGH);
    lcd_write_byte(data);
}

/* Initialize the 16x2 LCD */
void lcd_init(void)
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
    lcd_write_nibble(0x03);
    ms_delay(5);

    lcd_write_nibble(0x03);
    ms_delay(2);

    lcd_write_nibble(0x03);
    ms_delay(2);

    lcd_write_nibble(0x02);
    ms_delay(2);

    /* 4-bit mode, 2 lines, 5x8 font */
    lcd_command(0x28);

    /* Display ON, cursor OFF */
    lcd_command(0x0C);

    /* Entry mode: increment cursor */
    lcd_command(0x06);

    lcd_clear();
}

/* Clear the LCD */
void lcd_clear(void)
{
    lcd_command(0x01);
}

/* Position cursor: row 0 or 1, column 0 to 15 */
void lcd_set_cursor(uint8_t row, uint8_t col)
{
    if (row == 0)
    {
        lcd_command(0x80 + col);
    }
    else
    {
        lcd_command(0xC0 + col);
    }
}

/* Display a string */
void lcd_string(const char *str)
{
    while (*str != '\0')
    {
        lcd_data((uint8_t)*str);
        str++;
    }
}

/* Display an unsigned decimal number */
void lcd_print_number(uint16_t number)
{
    char digits[5];
    uint8_t i = 0;

    if (number == 0)
    {
        lcd_data('0');
        return;
    }

    while (number > 0 && i < 5)
    {
        digits[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0)
    {
        lcd_data(digits[--i]);
    }
}
