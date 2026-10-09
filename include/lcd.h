
#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* LCD connected to Port A */
#define LCD_PORT 'L' 
#define LCD_RS 0 
#define LCD_EN 1 
#define LCD_D4 2 
#define LCD_D5 3 
#define LCD_D6 4 
#define LCD_D7 5

void lcd_init(void);
void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void lcd_string(const char *str);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print_number(uint16_t number);

#endif
