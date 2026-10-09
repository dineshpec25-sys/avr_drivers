#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* LCD connected to Port K */
#define LCD_PORT 'K'
#define LCD_RS 0 /* PK0 / A8  */
#define LCD_EN 1 /* PK1 / A9  */
#define LCD_D4 4 /* PK4 / A12 */
#define LCD_D5 5 /* PK5 / A13 */
#define LCD_D6 6 /* PK6 / A14 */
#define LCD_D7 7 /* PK7 / A15 */

void lcd_init(void);
void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void lcd_string(const char *str);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print_number(uint16_t number);

#endif
