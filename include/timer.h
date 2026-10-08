#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* Timer0 - millisecond timing */

void timer_init(void);
void ms_delay(uint16_t ms);


/* Timer1 - high resolution measurement */

void timer_measure_reset(void);
void timer_measure_start(void);
void timer_measure_stop(void);
uint32_t timer_measure_get(void);

#endif
