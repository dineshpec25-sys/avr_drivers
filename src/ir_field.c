#include <stdint.h>

#include "../include/ir.h"
#include "../include/gpio.h"
#include "../hm/ir_regs.h"

void ir_init(void)
{
    gpio_mode(IR_PORT, IR_PIN, GPIO_INPUT_PULLUP);
}

slot_status_t ir_get_status_pin(uint8_t pin)
{
    uint8_t value = gpio_read(IR_PORT, pin);

#if IR_ACTIVE_LOW
    return (value == LOW) ? OCCUPIED : AVAILABLE;
#else
    return (value == HIGH) ? OCCUPIED : AVAILABLE;
#endif
}

slot_status_t ir_get_status(void)
{
    return ir_get_status_pin(IR_PIN);
}
