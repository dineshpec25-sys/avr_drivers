#include <avr/interrupt.h>

#include "../include/timer.h"
#include "../hm/timer_regs.h"


/*
 * Number of Timer1 overflows that occurred
 * during the current measurement.
 */
volatile uint32_t timer1_overflow = 0;


/* =========================================================
 * TIMER INITIALIZATION
 * ========================================================= */

void timer_init(void)
{
    /* =====================================================
     * TIMER 0
     *
     * CTC mode
     * Prescaler = 64
     *
     * 16 MHz / 64 = 250 kHz
     * 1 tick = 4 us
     *
     * OCR0A = 249
     *
     * 250 x 4 us = 1 ms
     * ===================================================== */

    HM_TCCR0A = 0x02;
    HM_TCCR0B = 0x03;

    HM_TCNT0 = 0;
    HM_OCR0A = 249;

    /* Clear Timer0 compare-match flag */
    HM_TIFR0 = (1 << 1);


    /* =====================================================
     * TIMER 1
     *
     * Normal mode
     * Initially stopped
     *
     * 16 MHz / 8 = 2 MHz
     * 1 count = 0.5 us
     * ===================================================== */

    HM_TCCR1A = 0x00;
    HM_TCCR1B = 0x00;

    HM_TCNT1 = 0;

    timer1_overflow = 0;

    /* Clear Timer1 overflow flag */
    HM_TIFR1 = (1 << 0);

    /* Disable Timer1 overflow interrupt initially */
    HM_TIMSK1 &= ~(1 << 0);
}


/* =========================================================
 * TIMER 0
 * ========================================================= */

/*
 * Delay for the requested number of milliseconds.
 */
void ms_delay(uint16_t ms)
{
    while (ms--)
    {
        /*
         * Wait for Timer0 compare match.
         */
        while (!(HM_TIFR0 & (1 << 1)))
        {
        }

        /*
         * Clear compare-match flag.
         */
        HM_TIFR0 = (1 << 1);
    }
}


/* =========================================================
 * TIMER 1
 * ========================================================= */

/*
 * Timer1 overflow ISR.
 *
 * Timer1:
 *
 * 0 -> 1 -> ... -> 65535 -> 0
 *                           ^
 *                      overflow
 *
 * Every overflow represents 65536 timer counts.
 */
ISR(TIMER1_OVF_vect)
{
    timer1_overflow++;
}


/*
 * Reset Timer1 measurement.
 */
void timer_measure_reset(void)
{
    /*
     * Stop Timer1.
     */
    HM_TCCR1B = 0x00;

    /*
     * Reset hardware counter.
     */
    HM_TCNT1 = 0;

    /*
     * Reset software overflow counter.
     */
    timer1_overflow = 0;

    /*
     * Clear pending overflow flag.
     */
    HM_TIFR1 = (1 << 0);
}


/*
 * Start Timer1.
 *
 * CS12 = 0
 * CS11 = 1
 * CS10 = 0
 *
 * Therefore:
 *
 * CPU / 8
 *
 * 16 MHz / 8 = 2 MHz
 * 1 count = 0.5 us
 */
void timer_measure_start(void)
{
    /*
     * Clear any pending overflow flag.
     */
    HM_TIFR1 = (1 << 0);

    /*
     * Enable Timer1 overflow interrupt.
     */
    HM_TIMSK1 |= (1 << 0);

    /*
     * Start Timer1 with /8 prescaler.
     */
    HM_TCCR1B = (1 << 1);
}


/*
 * Stop Timer1.
 */
void timer_measure_stop(void)
{
    /*
     * Stop Timer1.
     */
    HM_TCCR1B = 0x00;

    /*
     * Disable Timer1 overflow interrupt.
     */
    HM_TIMSK1 &= ~(1 << 0);
}


/*
 * Return the complete Timer1 count.
 *
 * total count =
 *
 *     overflow x 65536
 *     + current TCNT1
 *
 * Timer1 tick = 0.5 us
 */
uint32_t timer_measure_get(void)
{
    uint32_t overflow_count;
    uint16_t counter;
    uint8_t sreg_backup;

    /*
     * Prevent Timer1 overflow ISR from changing
     * timer1_overflow while we take the snapshot.
     */
    sreg_backup = SREG;
    cli();

    overflow_count = timer1_overflow;
    counter = HM_TCNT1;

    /*
     * Restore the previous global interrupt state.
     */
    SREG = sreg_backup;

    return (overflow_count * 65536UL) + counter;
}
