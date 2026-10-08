#include "../include/ir.h"
#include "../include/gpio.h"
#include "../hm/ir_regs.h"

void ir_init(void)
{
    gpio_mode(IR_PORT, IR_PIN, GPIO_INPUT);
}

slot_status_t ir_get_status(void)
{
    if (gpio_read(IR_PORT, IR_PIN) == LOW)
    {
        return OCCUPIED;
    }
    else
    {
        return AVAILABLE;
    }
}
