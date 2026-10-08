#ifndef TIMER_REGS_H
#define TIMER_REGS_H

#include <stdint.h>

/* ================= Timer 0 ================= */

#define HM_TCCR0A   (*(volatile uint8_t *)0x44)
#define HM_TCCR0B   (*(volatile uint8_t *)0x45)
#define HM_TCNT0    (*(volatile uint8_t *)0x46)
#define HM_OCR0A    (*(volatile uint8_t *)0x47)
#define HM_TIFR0    (*(volatile uint8_t *)0x35)


/* ================= Timer 1 ================= */

#define HM_TCCR1A   (*(volatile uint8_t *)0x80)
#define HM_TCCR1B   (*(volatile uint8_t *)0x81)
#define HM_TCNT1    (*(volatile uint16_t *)0x84)
#define HM_OCR1A    (*(volatile uint16_t *)0x88)
#define HM_TIFR1    (*(volatile uint8_t *)0x36)
#define HM_TIMSK1   (*(volatile uint8_t *)0x6F)

#endif
