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

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include "../include/pwm.h"
#include "../hm/pwm_regs.h"

#define PB4 4

void pwm_tone(uint16_t frequency_hz)
{
    static const uint16_t prescaler[] =
    {
        1, 8, 32, 64, 128, 256, 1024
    };

    /*
     * Timer2 clock-select bits:
     * /1    = 001
     * /8    = 010
     * /32   = 011
     * /64   = 100
     * /128  = 101
     * /256  = 110
     * /1024 = 111
     */
    static const uint8_t clock_bits[] =
    {
        1, 2, 3, 4, 5, 6, 7
    };

    uint8_t i;
    uint32_t top;

    if (frequency_hz == 0)
    {
        pwm_mute();
        return;
    }

    /* Find a suitable Timer2 prescaler and compare value. */
    for (i = 0; i < 7; i++)
    {
        top = F_CPU /
              (2UL * prescaler[i] * frequency_hz);

        if (top >= 1 && top <= 256)
            break;
    }

    /* Requested frequency cannot be represented. */
    if (i == 7)
    {
        pwm_mute();
        return;
    }

    /* Ensure PB4/OC2A is configured as an output. */
    HM_DDRB |= (1 << PB4);

    /* Stop Timer2 while configuring it. */
    HM_TCCR2B = 0;

    /*
     * CTC mode:
     * WGM21 = 1
     *
     * Toggle OC2A on compare match:
     * COM2A1:0 = 01
     */
    HM_TCCR2A = (1 << 6) | (1 << 1);

    HM_TCNT2 = 0;
    HM_OCR2A = (uint8_t)(top - 1);

    /* Start Timer2 with the selected prescaler. */
    HM_TCCR2B = clock_bits[i];
}

void pwm_mute(void)
{
    /* Stop Timer2. */
    HM_TCCR2B = 0;

    /* Disconnect OC2A from the timer. */
    HM_TCCR2A = 0;

    HM_OCR2A = 0;
}
