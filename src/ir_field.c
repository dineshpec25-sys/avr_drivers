#include "../include/ir.h"
#include "../include/gpio.h"
#include "../hm/ir_regs.h"


/* =========================================================
 * IR Sensor Initialization
 * ========================================================= */

void ir_init(void)
{
    /*
     * IR sensor output -> Input
     */
    gpio_mode(IR_PORT, IR_PIN, GPIO_INPUT);
}


/* =========================================================
 * Get Parking Slot Status
 * ========================================================= */

slot_status_t ir_get_status(void)
{
#if IR_ACTIVE_LOW

    /*
     * Active LOW sensor:
     *
     * LOW  -> Vehicle present
     * HIGH -> Vehicle absent
     */

    if (gpio_read(IR_PORT, IR_PIN) == LOW)
    {
        return OCCUPIED;
    }
    else
    {
        return AVAILABLE;
    }

#else

    /*
     * Active HIGH sensor:
     *
     * HIGH -> Vehicle present
     * LOW  -> Vehicle absent
     */

    if (gpio_read(IR_PORT, IR_PIN) == HIGH)
    {
        return OCCUPIED;
    }
    else
    {
        return AVAILABLE;
    }

#endif
}
