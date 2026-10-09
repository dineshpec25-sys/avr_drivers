#include <stdint.h>

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_init(void);
void pwm_start(void);
void pwm_stop(void);
void pwm_set_duty(uint8_t duty);
void pwm_tone(uint16_t frequency_hz);
void pwm_mute(void);

#endif
