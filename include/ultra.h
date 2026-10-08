#ifndef ULTRA_H
#define ULTRA_H

#include <stdint.h>
#include "gpio.h"

#define ULTRA_PORT      GPIO_PORT_J
#define ULTRA_TRIG_PIN  0
#define ULTRA_ECHO_PIN  1

void ultra_init(void);
void ultra_trigger(void);
uint32_t ultra_get_echo_time_us(void);
uint16_t ultra_get_distance_cm(void);

#endif
