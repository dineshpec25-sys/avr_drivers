#include <stdint.h>

#include "gpio.h"
#include "timer.h"
#include "ultra.h"


/* =========================================================
 * Ultrasonic Initialization
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
 * Generate Ultrasonic Trigger Pulse
 * ========================================================= */

void ultra_trigger(void)
{
    /*
     * Make sure TRIG is LOW first
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, LOW);

    /*
     * Start Timer1
     */

    timer_measure_start();

    /*
     * TRIG HIGH
     */

    gpio_write(ULTRA_PORT, ULTRA_TRIG_PIN, HIGH);

    /*
     * Keep TRIG HIGH for 10 us
     */

    while(timer_measure_get() < 10);

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
    uint32_t timeout;

    /*
     * Generate 10 us trigger pulse
     */

    ultra_trigger();

    /*
     * Start Timer1
     *
     * We use it for timeout while waiting
     * for ECHO to become HIGH.
     */

    timer_measure_start();

    /*
     * Wait for ECHO rising edge
     */

    while(gpio_read(ULTRA_PORT, ULTRA_ECHO_PIN) == LOW)
    {
        timeout = timer_measure_get();

        /*
         * 30 ms timeout
         *
         * This prevents the program from getting
         * stuck forever if no ECHO is received.
         */

        if(timeout >= 30000)
        {
            timer_measure_stop();
            return 0;
        }
    }


    /*
     * ECHO has become HIGH.
     *
     * Reset Timer1 now so that timing starts
     * exactly from the ECHO rising edge.
     */

    timer_measure_start();


    /*
     * Wait until ECHO becomes LOW.
     */

    while(gpio_read(ULTRA_PORT, ULTRA_ECHO_PIN) == HIGH)
    {
        timeout = timer_measure_get();

        /*
         * Safety timeout
         */

        if(timeout >= 30000)
        {
            timer_measure_stop();
            return 0;
        }
    }


    /*
     * ECHO has become LOW.
     *
     * Timer value = ECHO HIGH duration.
     */

    timeout = timer_measure_get();

    timer_measure_stop();

    return timeout;
}


/* =========================================================
 * Calculate Distance
 * ========================================================= */

uint16_t ultra_get_distance_cm(void)
{
    uint32_t echo_time_us;
    uint16_t distance_cm;


    /*
     * Get ECHO pulse width
     */

    echo_time_us = ultra_get_echo_time_us();


    /*
     * No valid ECHO
     */

    if(echo_time_us == 0)
    {
        return 0;
    }


    /*
     * Ultrasonic distance formula:
     *
     * Distance(cm) = Echo_Time(us) / 58
     *
     * Because the measured time is for:
     *
     * sensor -> object -> sensor
     */

    distance_cm = echo_time_us / 58;


    return distance_cm;
}
