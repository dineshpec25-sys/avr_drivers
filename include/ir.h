#include <stdint.h>

typedef enum {
    AVAILABLE,
    OCCUPIED
} slot_status_t;

void ir_init(void);
slot_status_t ir_get_status(void);
slot_status_t ir_get_status_pin(uint8_t pin);
