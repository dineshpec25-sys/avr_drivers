#ifndef SEG7_H
#define SEG7_H

#include <stdint.h>

typedef struct
{
    char port;
    uint8_t pin;
} seg7_pin_t;

typedef struct
{
    seg7_pin_t a;
    seg7_pin_t b;
    seg7_pin_t c;
    seg7_pin_t d;
    seg7_pin_t e;
    seg7_pin_t f;
    seg7_pin_t g;
} seg7_map_t;

void seg7_init(seg7_map_t map);
void seg7_clear(void);
void seg7_display(uint8_t num);

#endif
