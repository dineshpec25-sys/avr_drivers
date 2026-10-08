#include "../include/gpio.h"
#include "../hm/gpio_regs.h"

int gpio_validate(char port, uint8_t pin)
{
    if (pin > 7 ||
        port == 'I' || port > 'L' || port < 'A')
        return 1;

    return 0;
}

void gpio_mode(char port, uint8_t pin, gpio_mode_t mode)
{
    if (gpio_validate(port, pin))
        return;

    gpio_port_t port_id;

    if (port > 'I')
        port_id = port - 'A' - 1;
    else
        port_id = port - 'A';

    gpio_regs_t *gpio = gpio_ports[port_id];

    if (mode == GPIO_OUTPUT)
    {
        gpio->ddr |= (1 << pin);
    }
    else if (mode == GPIO_INPUT)
    {
        gpio->ddr &= ~(1 << pin);
        gpio->port &= ~(1 << pin);
    }
    else if (mode == GPIO_INPUT_PULLUP)
    {
        gpio->ddr &= ~(1 << pin);
        gpio->port |= (1 << pin);
    }
}

void gpio_write(char port, uint8_t pin, gpio_write_t value)
{
    if (gpio_validate(port, pin))
        return;

    gpio_port_t port_id;

    if (port > 'I')
        port_id = port - 'A' - 1;
    else
        port_id = port - 'A';

    gpio_regs_t *gpio = gpio_ports[port_id];

    if (value == HIGH)
    {
        gpio->port |= (1 << pin);
    }
    else if (value == LOW)
    {
        gpio->port &= ~(1 << pin);
    }
}

gpio_write_t gpio_read(char port, uint8_t pin)
{
    if (gpio_validate(port, pin))
        return LOW;

    gpio_port_t port_id;

    if (port > 'I')
        port_id = port - 'A' - 1;
    else
        port_id = port - 'A';

    gpio_regs_t *gpio = gpio_ports[port_id];

    if (gpio->pin & (1 << pin))
        return HIGH;

    return LOW;
}
