#include "../include/pwm.h"
#include "../hm/pwm_regs.h"

#define PB4 4

void pwm_init(void)
{
    HM_DDRB |= (1 << PB4); // setting the PWM pin D10 as a output

    /* Timer2:
       Fast PWM
       TOP = 0xFF
       Non-inverting OC2A
    */
    HM_TCCR2A = (1 << 7) | (1 << 1) | (1 << 0);
    HM_TCCR2B = 0x00; // setting the timer to initally zero
    HM_OCR2A = 128; // 128/256 * 100 = 50%
    HM_TCNT2 = 0; // reset the counter
}

void pwm_start(void)
{
    HM_TCCR2B = (1 << 2); // perscaler 64
}

void pwm_stop(void)
{
    HM_TCCR2B &= ~((1 << 2) | (1 << 1) | (1 << 0)); // to stop the timer
}

void pwm_set_duty(uint8_t duty)
{
    if (duty > 100)
        duty = 100;

    HM_OCR2A = ((uint16_t)duty * 255) / 100;
}
