#include <stdint.h>

#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

typedef enum
{
	GPIO_PORT_A=0,
	GPIO_PORT_B=1,
	GPIO_PORT_C=2,
	GPIO_PORT_D=3,
	GPIO_PORT_E=4,
	GPIO_PORT_F=5,
	GPIO_PORT_G=6,
	GPIO_PORT_H=7,
	GPIO_PORT_J=8,
	GPIO_PORT_K=9,
	GPIO_PORT_L=10
}gpio_port_t;

typedef enum
{
	GPIO_INPUT,
	GPIO_OUTPUT,
	GPIO_INPUT_PULLUP
}gpio_mode_t;

typedef enum
{
	HIGH,
	LOW
}gpio_write_t;

void gpio_mode(char PORT, uint8_t PIN, gpio_mode_t MODE);
void gpio_write(char PORT, uint8_t PIN, gpio_write_t value);
gpio_write_t gpio_read(char PORT, uint8_t PIN);


#endif
