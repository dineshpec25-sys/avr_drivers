#include <stdint.h>

#include "../../../include/adc.h"
#include "../../../include/pwm.h"

int main(void)
{
    uint16_t adc_value;
    uint8_t duty;

    adc_init();

    pwm_init();
    pwm_start();

    while (1)
    {
        /* Read the analog IR sensor through ADC0 */
        adc_value = adc_read(0);

        /* Convert ADC range 0–1023 to PWM duty range 0–100 */
        duty = (uint8_t)((adc_value * 100) / 1023);

        /* Set LED brightness */
        pwm_set_duty(duty);
    }
}
