#include <stdint.h>
#include "../include/timer.h"
#include "../hm/timer_regs.h"

static uint32_t timer1_overflow = 0;

void timer_init(void)
{
    /* TIMER 0
     * CTC mode
     * Prescaler = 64
     * 16 MHz / 64 = 250 kHz
     * 1 tick = 4 us
     * OCR0A = 249
     * 250 x 4 us = 1 ms
     */

    HM_TCCR0A = 0x02; // Timer0's control register A we configer it to CTC mode
    HM_TCCR0B = 0x03; // Timer0's control register B we configer it to perscaler of 64

    HM_TCNT0 = 0; // the actual counter we zero it for safety
    HM_OCR0A = 249; // the reference or compare register we set to 249 (0 - 249) = 250

    HM_TIFR0 = (1 << 1); // It is interrupt flag register


    /* TIMER 1
     * Normal mode
     * Initially stopped
     * 16 MHz / 8 = 2 MHz
     * 1 count = 0.5 us
     */

    HM_TCCR1A = 0x00; // it is configered to Normal mode
    HM_TCCR1B = 0x00; // it is initially timer1 is not connected

    HM_TCNT1 = 0; // making it zero for safety

    timer1_overflow = 0;
    HM_TIFR1 = (1 << 0); // It is interrupt flag
    HM_TIMSK1 &= ~(1 << 0); // timer overflow interrupt setting it to zero
}
// Timer0 for 1ms delay
void ms_delay(uint16_t ms)
{
    /* Timer0 runs freely, so the compare flag may already be set.
     * Restart the count so the first millisecond is a full one. */
    HM_TCNT0 = 0;
    HM_TIFR0 = (1 << 1);

    while (ms--)
    {
        while (!(HM_TIFR0 & (1 << 1)))
        {

        }
        HM_TIFR0 = (1 << 1); // clearing the interrupt flag;
    }
}

/* every time a overflow is detected
 * the interrupt will be generated which
 * will increment the timer1_overflow++
 * */

void timer_measure_reset(void)
{
    HM_TCCR1B = 0x00; // stopping the timer by disconnecting the clock
    HM_TCNT1 = 0; // making the counter zero
    timer1_overflow = 0; // resetting the overflow
    HM_TIFR1 = (1 << 0); // resetting the interrupt flag
}


/*
 * Start Timer1.
 *
 * CS12 = 0
 * CS11 = 1
 * CS10 = 0
 *
 * CPU / 8
 * 16 MHz / 8 = 2 MHz
 * 1 count = 0.5 us
 */
void timer_measure_start(void)
{
    HM_TIFR1 = (1 << 0);       // Clear overflow flag
    HM_TIMSK1 &= ~(1 << 0);    // Disable Timer1 overflow interrupt
    HM_TCCR1B = (1 << 1);      // Start Timer1 with prescaler 8
}
void timer_measure_stop(void)
{
    HM_TCCR1B = 0x00; // disconnecting the clock
    HM_TIMSK1 &= ~(1 << 0); // disabling the interrupt
}


/*
 * Return the complete Timer1 count.
 *
 * total count = overflow x 65536 + current TCNT1
 *
 * Timer1 tick = 0.5 us
 *
 * This function is to calcualte the total time taken for the 
 * echo 
 * using uint32_t as a return type because the ticks can be upto 65535
 * */
uint32_t timer_measure_get(void)
{
    uint16_t counter;

    counter = HM_TCNT1;

    if (HM_TIFR1 & (1 << 0))
    {
        timer1_overflow++;
        HM_TIFR1 = (1 << 0);  // Clear TOV1 by writing 1
        counter = HM_TCNT1;
    }

    return (timer1_overflow * 65536UL) + counter; //UL indicates that the operation must be in the unsigned long
}
