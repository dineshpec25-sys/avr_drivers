#include "../include/adc.h"
#include "../hm/adc_regs.h"

void adc_init(void)
{
    HM_ADMUX = (1 << 6); // this register says the reference voltage, ADC channel
			 // that sets ref. voltage as AVCC 
			 // this means the result is right-adjusted
    HM_ADCSRA = (1 << 7) | (1 << 2) | (1 << 1) | (1 << 0);
    /* It enables the ADC (1 << 7) the remaining three bit to set prescaler
     * we choose 128 perscalar because it give 125kHz of ADC clock speed 
     * slower clock speed gives sufficient time for ADC conversion*/
}

uint16_t adc_read(uint8_t channel)
{
    uint16_t result;
    HM_ADMUX = (HM_ADMUX & 0xE0) | (channel & 0x1F); // to select ADC channel 
    HM_ADCSRA |= (1 << 6); // to start the conversion
    while (HM_ADCSRA & (1 << 6))
    {
	    /* Waiting until the the conversion in ADC*/
    }

    /* Read ADC result */
    result = HM_ADCL; // the actuall place where we will read the output is 10-bit but the register is only 
		      // 8-bit to we use 2 register ADCH and ADCL it is right-adjusted
    result |= ((uint16_t)HM_ADCH << 8); // reading the remaining two digits from the ADCH

    return result;
}
