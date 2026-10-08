#ifndef ADC_REGS_H
#define ADC_REGS_H

#include <stdint.h>

/* ADC registers */
#define HM_ADMUX    (*(volatile uint8_t *)0x7C) // ADC multiplexer selection register
#define HM_ADCSRA   (*(volatile uint8_t *)0x7A) // ADC control and status register 
#define HM_ADCSRB   (*(volatile uint8_t *)0x7B) // ADC control and status register
#define HM_ADCL     (*(volatile uint8_t *)0x78) // ADC Data register
#define HM_ADCH     (*(volatile uint8_t *)0x79) // same as the ADC data register they store the value after the conversion.

#endif
