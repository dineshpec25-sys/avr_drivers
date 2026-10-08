#include <stdint.h>

#ifndef GPIO_REGS_H
#define GPIO_REGS_H

typedef struct
{
    volatile uint8_t pin;
    volatile uint8_t ddr;
    volatile uint8_t port;
} gpio_regs_t;

/* The above struct will act as a framework to assign 
 * the address of the port, pin, ddr automatically 
 * because in hardware they are near to each other like 
 * 0x20 is PINA and 0x21 is DDRA and 0x22 is PORTA with 
 * the help of struct the address are assigned */

#define GPIO_A ((gpio_regs_t *)0x20)
#define GPIO_B ((gpio_regs_t *)0x23)
#define GPIO_C ((gpio_regs_t *)0x26)
#define GPIO_D ((gpio_regs_t *)0x29)
#define GPIO_E ((gpio_regs_t *)0x2C)
#define GPIO_F ((gpio_regs_t *)0x2F)
#define GPIO_G ((gpio_regs_t *)0x32)
#define GPIO_H ((gpio_regs_t *)0x100)
#define GPIO_J ((gpio_regs_t *)0x103)
#define GPIO_K ((gpio_regs_t *)0x106)
#define GPIO_L ((gpio_regs_t *)0x109)

/* The above defines the address to the strcut assign the pin
 * ddr and port.*/

static gpio_regs_t *const gpio_ports[] =
{
    GPIO_A,
    GPIO_B,
    GPIO_C,
    GPIO_D,
    GPIO_E,
    GPIO_F,
    GPIO_G,
    GPIO_H,
    GPIO_J,
    GPIO_K,
    GPIO_L
};

/* The above array will map the struct with the enum and the 
 * gpio_reg_t together we use static because it is a header file
 * and to avoid linker error and multiple definition if the array
 * and used const to avoid any change in the value of the pointer 
 *array.*/

#endif
