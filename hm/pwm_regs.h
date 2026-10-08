#ifndef PWM_REGS_H
#define PWM_REGS_H

#include <stdint.h>

/* Timer2 registers */
#define HM_TCCR2A   (*(volatile uint8_t *)0xB0)
#define HM_TCCR2B   (*(volatile uint8_t *)0xB1)
#define HM_TCNT2    (*(volatile uint8_t *)0xB2)
#define HM_OCR2A    (*(volatile uint8_t *)0xB3)

/* PORTB registers */
#define HM_DDRB     (*(volatile uint8_t *)0x24)
#define HM_PORTB    (*(volatile uint8_t *)0x25)

#endif
