#include "../include/led.h"
#include "../include/gpio.h"
#include "../hm/led_regs.h"

void led_init(void)
{
    gpio_mode(LED_PORT, LED_PIN, GPIO_OUTPUT);
}

void led_on(void)
{
    gpio_write(LED_PORT, LED_PIN, HIGH);
}

void led_off(void)
{
    gpio_write(LED_PORT, LED_PIN, LOW);
}

void led_toggle(void)
{
    if (gpio_read(LED_PORT, LED_PIN) == HIGH)
        gpio_write(LED_PORT, LED_PIN, LOW);
    else
        gpio_write(LED_PORT, LED_PIN, HIGH);
}
