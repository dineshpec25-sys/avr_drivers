#include <stdint.h>

#include "gpio.h"
#include "timer.h"
#include "ultra.h"


/* =========================================================
 * Ultrasonic Sensor Initialization
 * ========================================================= */

void ultra_init(void)
{
    /*
     * TRIG -> Output
     * ECHO -> Input
     */

    gpio_mode(ULTRA_PORT, ULTRA_TRIG_PIN, GPIO_OUTPUT);
    gpio_mode(ULTRA_PORT, ULTRA_ECHO_PIN, GPIO_INPUT);

    /*
     * Keep TRIG LOW initially
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, LOW);
}


/* =========================================================
 * Generate 10 us Trigger Pulse
 * ========================================================= */

void ultra_trigger(void)
{
    /*
     * Make sure TRIG is LOW first
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, LOW);

    /*
     * Reset Timer1.
     *
     * Timer1 runs at:
     *
     * 16 MHz / 8 = 2 MHz
     *
     * Therefore:
     *
     * 1 count = 0.5 us
     */

    timer_measure_reset();

    /*
     * Start Timer1
     */

    timer_measure_start();

    /*
     * TRIG HIGH
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, HIGH);

    /*
     * 10 us / 0.5 us = 20 counts
     */

    while (timer_measure_get() < 20)
    {
    }

    /*
     * TRIG LOW
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, LOW);

    /*
     * Stop Timer1
     */

    timer_measure_stop();
}


/* =========================================================
 * Measure ECHO Pulse Width
 * ========================================================= */

uint32_t ultra_get_echo_time_us(void)
{
    uint32_t timer_count;

    /*
     * Generate 10 us trigger pulse
     */

    ultra_trigger();


    /* -----------------------------------------------------
     * Wait for ECHO to become HIGH
     * ----------------------------------------------------- */

    /*
     * Reset Timer1 before starting the timeout measurement.
     */

    timer_measure_reset();

    timer_measure_start();

    while (gpio_read(ULTRA_PORT, ULTRA_ECHO_PIN) == LOW)
    {
        timer_count = timer_measure_get();

        /*
         * 30 ms timeout.
         *
         * Timer1:
         *
         * 1 count = 0.5 us
         *
         * 30,000 us / 0.5 us
         * = 60,000 counts
         */

        if (timer_count >= 60000)
        {
            timer_measure_stop();

            return 0;
        }
    }


    /* -----------------------------------------------------
     * ECHO is HIGH
     * Start measuring the actual ECHO pulse width.
     * ----------------------------------------------------- */

    timer_measure_reset();

    timer_measure_start();


    /* -----------------------------------------------------
     * Wait for ECHO to become LOW
     * ----------------------------------------------------- */

    while (gpio_read(ULTRA_PORT, ULTRA_ECHO_PIN) == HIGH)
    {
        timer_count = timer_measure_get();

        /*
         * Safety timeout.
         *
         * Prevents the program from getting stuck if
         * ECHO remains HIGH.
         */

        if (timer_count >= 60000)
        {
            timer_measure_stop();

            return 0;
        }
    }


    /*
     * Get the number of Timer1 counts.
     */

    timer_count = timer_measure_get();

    /*
     * Stop Timer1.
     */

    timer_measure_stop();


    /*
     * Timer1:
     *
     * 1 count = 0.5 us
     *
     * Therefore:
     *
     * time_us = timer_count / 2
     */

    return timer_count / 2;
}


/* =========================================================
 * Calculate Distance
 * ========================================================= */

uint16_t ultra_get_distance_cm(void)
{
    uint32_t echo_time_us;

    /*
     * Get ECHO pulse width in microseconds.
     */

    echo_time_us = ultra_get_echo_time_us();


    /*
     * No valid ECHO received.
     */

    if (echo_time_us == 0)
    {
        return 0;
    }


    /*
     * Ultrasonic distance formula:
     *
     * Distance(cm) = Echo_Time(us) / 58
     *
     * The measured ECHO time represents:
     *
     * Sensor -> Object -> Sensor
     */

    return echo_time_us / 58;
}
