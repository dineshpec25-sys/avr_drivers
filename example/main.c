#include <stdint.h>
#include <stdio.h>

#include "../include/gpio.h"
#include "../include/timer.h"
#include "../include/ir.h"
#include "../include/ultra.h"
#include "../include/lcd.h"
#include "../include/pwm.h"

/* IR sensor connections */
#define IR1_PIN 2       /* PD2 / Mega D19 */
#define IR2_PIN 3       /* PD3 / Mega D18 */

/* LCD mode switch */
#define BUTTON_PORT 'E'
#define BUTTON_PIN  4   /* PE4 / Mega D2 */

/* Status LEDs */
#define LED_PORT  'G'
#define LED_GREEN 0     /* PG0 / Mega D41 */
#define LED_YELLOW 1    /* PG1 / Mega D40 */
#define LED_RED   2     /* PG2 / Mega D39 */

#define LOOP_DELAY_MS 10

typedef enum
{
    MODE_SLOTS,
    MODE_DISTANCE,
    MODE_DURATION,
    MODE_COUNT
} lcd_mode_t;

typedef enum
{
    SAFE,
    CAUTION,
    WARNING,
    STOP,
    SENSOR_ERROR
} distance_state_t;

static lcd_mode_t lcd_mode = MODE_SLOTS;

/*
 * Software time counter.
 * Because ultrasonic measurement is blocking, this is approximate.
 */
static uint32_t elapsed_ms = 0;

static uint32_t slot_seconds[2] = {0, 0};
static uint16_t slot_ms[2] = {0, 0};

static uint8_t previous_occupied[2] = {0, 0};

/* Button debounce */
static uint8_t previous_raw = HIGH;
static uint8_t stable_button = HIGH;
static uint32_t button_changed_ms = 0;

/* Buzzer timing */
static uint32_t beep_start_ms = 0;
static uint16_t active_frequency = 0;
static uint8_t buzzer_on = 0;
static distance_state_t previous_state = SENSOR_ERROR;

/* LCD timing */
static uint32_t lcd_refresh_ms = 0;
static uint32_t duration_page_ms = 0;
static uint8_t duration_page = 0;


/* Write exactly 16 characters to one LCD row. */
static void lcd_write_line(uint8_t row, const char *text)
{
    uint8_t i = 0;

    lcd_set_cursor(row, 0);

    while (text[i] != '\0' && i < 16)
    {
        lcd_data((uint8_t)text[i]);
        i++;
    }

    while (i < 16)
    {
        lcd_data(' ');
        i++;
    }
}


/* Read both IR sensors independently. */
static uint8_t slot_occupied(uint8_t pin)
{
    return (ir_get_status_pin(pin) == OCCUPIED);
}


/* 3-sample median filter to reject ultrasonic noise and random dropouts */
static uint16_t get_filtered_distance(void)
{
    static uint16_t history[3] = {0, 0, 0};
    static uint8_t idx = 0;
    static uint8_t count = 0;

    uint16_t raw = ultra_get_distance_cm();

    history[idx] = raw;
    idx = (idx + 1) % 3;
    if (count < 3)
        count++;

    if (count < 3)
        return raw;

    uint16_t a = history[0];
    uint16_t b = history[1];
    uint16_t c = history[2];

    /* Median of 3 values */
    if ((a <= b && b <= c) || (c <= b && b <= a))
        return b;
    if ((b <= a && a <= c) || (c <= a && a <= b))
        return a;
    return c;
}

/* Convert the measured distance into a warning state with hysteresis to prevent flickering. */
static distance_state_t get_distance_state(uint16_t distance, distance_state_t current)
{
    /* 0 indicates no echo received within timeout (clear path / out of range) */
    if (distance == 0)
        return SAFE;

    switch (current)
    {
        case STOP:
            /* To exit STOP, obstacle must move beyond 7 cm */
            if (distance > 7)
            {
                if (distance > 30)
                    return SAFE;
                if (distance > 15)
                    return CAUTION;
                return WARNING;
            }
            return STOP;

        case WARNING:
            if (distance <= 5)
                return STOP;
            if (distance > 17) /* Hysteresis: exit WARNING above 17 cm */
            {
                if (distance > 30)
                    return SAFE;
                return CAUTION;
            }
            return WARNING;

        case CAUTION:
            if (distance <= 15)
            {
                if (distance <= 5)
                    return STOP;
                return WARNING;
            }
            if (distance > 33) /* Hysteresis: exit CAUTION above 33 cm */
                return SAFE;
            return CAUTION;

        case SAFE:
        default:
            if (distance <= 5)
                return STOP;
            if (distance <= 15)
                return WARNING;
            if (distance <= 30)
                return CAUTION;
            return SAFE;
    }
}
/* Update the three status LEDs. */
static void update_leds(distance_state_t state)
{
    static distance_state_t last_state;
    static uint8_t initialized = 0;

    /* Avoid rewriting the LED outputs when nothing changed. */
    if (initialized && state == last_state)
        return;

    initialized = 1;
    last_state = state;

    /* Turn all LEDs off before selecting the new indicator. */
    gpio_write(LED_PORT, LED_GREEN, LOW);
    gpio_write(LED_PORT, LED_YELLOW, LOW);
    gpio_write(LED_PORT, LED_RED, LOW);

    switch (state)
    {
        case SAFE:
            gpio_write(LED_PORT, LED_GREEN, HIGH);
            break;

        case CAUTION:
            gpio_write(LED_PORT, LED_YELLOW, HIGH);
            break;

        case WARNING:
        case STOP:
        case SENSOR_ERROR:
        default:
            gpio_write(LED_PORT, LED_RED, HIGH);
            break;
    }
}

/* Track the duration of each occupied slot with noise filter. */
static void update_slot_duration(uint8_t occupied1,
                                 uint8_t occupied2)
{
    uint8_t occupied[2] = {occupied1, occupied2};
    static uint16_t vacant_ms[2] = {0, 0};
    uint8_t i;

    for (i = 0; i < 2; i++)
    {
        if (occupied[i])
        {
            vacant_ms[i] = 0;

            if (!previous_occupied[i])
            {
                /* Start timing when a slot becomes occupied. */
                slot_seconds[i] = 0;
                slot_ms[i] = 0;
                previous_occupied[i] = 1;
            }
            else
            {
                slot_ms[i] += LOOP_DELAY_MS;

                if (slot_ms[i] >= 1000)
                {
                    slot_ms[i] -= 1000;
                    slot_seconds[i]++;
                }
            }
        }
        else
        {
            /* If previously occupied, filter out brief glitches before resetting */
            if (previous_occupied[i])
            {
                vacant_ms[i] += LOOP_DELAY_MS;
                if (vacant_ms[i] >= 1000) /* Vacant for 1 full second */
                {
                    slot_seconds[i] = 0;
                    slot_ms[i] = 0;
                    vacant_ms[i] = 0;
                    previous_occupied[i] = 0;
                }
            }
            else
            {
                slot_seconds[i] = 0;
                slot_ms[i] = 0;
            }
        }
    }
}


/* Change the LCD page once for every button press. */
static void update_button(void)
{
    uint8_t raw = gpio_read(BUTTON_PORT, BUTTON_PIN);

    if (raw != previous_raw)
    {
        previous_raw = raw;
        button_changed_ms = elapsed_ms;
    }

    if ((uint32_t)(elapsed_ms - button_changed_ms) >= 30 &&
        raw != stable_button)
    {
        stable_button = raw;

        if (stable_button == LOW)
        {
            lcd_mode = (lcd_mode + 1) % MODE_COUNT;
            lcd_refresh_ms = 0;
        }
    }
}


/*
 * Beeper patterns:
 * SAFE:        500 Hz, slow beep
 * CAUTION:     800 Hz, moderate beep
 * WARNING:    1200 Hz, fast beep
 * STOP:       1800 Hz, continuous tone
 * SENSOR_ERROR: fast warning beeps
 */
static void update_buzzer(distance_state_t state, uint16_t distance)
{
    uint16_t frequency;
    uint16_t interval;
    uint16_t on_time;
    uint8_t continuous = 0;
    uint32_t phase;

    /* When clear (no obstacle) and safe, keep buzzer silent */
    if (state == SAFE && distance == 0)
    {
        if (buzzer_on)
        {
            pwm_mute();
            buzzer_on = 0;
            active_frequency = 0;
        }
        previous_state = state;
        return;
    }

    switch (state)
    {
        case SAFE:
            frequency = 500;
            interval = 1000;
            on_time = 80;
            break;

        case CAUTION:
            frequency = 800;
            interval = 500;
            on_time = 100;
            break;

        case WARNING:
            frequency = 1200;
            interval = 200;
            on_time = 100;
            break;

        case STOP:
            frequency = 1800;
            interval = 1;
            on_time = 1;
            continuous = 1;
            break;

        case SENSOR_ERROR:
        default:
            frequency = 1200;
            interval = 150;
            on_time = 70;
            break;
    }

    /* Restart the beep cycle when the warning state changes. */
    if (state != previous_state)
    {
        beep_start_ms = elapsed_ms;
        previous_state = state;
    }

    if (continuous)
    {
        if (!buzzer_on || active_frequency != frequency)
        {
            pwm_tone(frequency);
            buzzer_on = 1;
            active_frequency = frequency;
        }

        return;
    }

    phase = elapsed_ms - beep_start_ms;

    if (phase >= interval)
    {
        beep_start_ms = elapsed_ms;
        phase = 0;
    }

    if (phase < on_time)
    {
        if (!buzzer_on || active_frequency != frequency)
        {
            pwm_tone(frequency);
            buzzer_on = 1;
            active_frequency = frequency;
        }
    }
    else if (buzzer_on)
    {
        pwm_mute();
        buzzer_on = 0;
        active_frequency = 0;
    }
}


/* LCD mode 1: display parking-slot availability. */
static void show_slot_mode(uint8_t occupied1,
                           uint8_t occupied2)
{
    char line1[17];
    const char *line2;

    snprintf(line1, sizeof(line1),
             "S1:%s S2:%s",
             occupied1 ? "FULL" : "FREE",
             occupied2 ? "FULL" : "FREE");

    if (occupied1 && occupied2)
        line2 = "NO SLOT AVAIL.";
    else if (occupied1)
        line2 = "PLEASE USE S2";
    else if (occupied2)
        line2 = "PLEASE USE S1";
    else
        line2 = "USE SLOT 1 OR 2";

    lcd_write_line(0, line1);
    lcd_write_line(1, line2);
}


/* LCD mode 2: display reversing distance and warning state. */
static void show_distance_mode(uint16_t distance,
                               distance_state_t state,
                               uint8_t occupied1)
{
    char line1[17];
    const char *line2;

    if (occupied1)
    {
        lcd_write_line(0, "SLOT 1: PARKED");
        lcd_write_line(1, "STOPPED");
        return;
    }

    switch (state)
    {
        case SAFE:
            line2 = "SAFE";
            break;

        case CAUTION:
            line2 = "CAUTION";
            break;

        case WARNING:
            line2 = "WARNING";
            break;

        case STOP:
            line2 = "STOP";
            break;

        default:
            line2 = "SENSOR ERROR";
            break;
    }

    if (state == SENSOR_ERROR)
        snprintf(line1, sizeof(line1), "DIST: SENSOR ERR");
    else if (distance == 0)
        snprintf(line1, sizeof(line1), "DIST: CLEAR");
    else
        snprintf(line1, sizeof(line1),
                 "DIST: %u cm", distance);

    lcd_write_line(0, line1);
    lcd_write_line(1, line2);
}


/* LCD mode 3: display occupied-slot duration. */
static void show_duration_mode(uint8_t occupied1,
                               uint8_t occupied2)
{
    uint8_t slot;
    uint32_t seconds;
    uint32_t hours;
    uint32_t minutes;

    char line1[17];
    char line2[17];

    if (!occupied1 && !occupied2)
    {
        lcd_write_line(0, "NO OCCUPIED SLOT");
        lcd_write_line(1, "TIME: 00:00:00");
        return;
    }

    if (occupied1 && occupied2)
    {
        if ((uint32_t)(elapsed_ms - duration_page_ms) >= 2000)
        {
            duration_page ^= 1;
            duration_page_ms = elapsed_ms;
        }

        slot = duration_page;
    }
    else
    {
        slot = occupied1 ? 0 : 1;
    }

    seconds = slot_seconds[slot];

    hours = seconds / 3600UL;
    minutes = (seconds / 60UL) % 60UL;
    seconds %= 60UL;

    /* Fixed strings avoid snprintf truncation warnings on the 16x2 LCD. */
    if (slot == 0)
        snprintf(line1, sizeof(line1), "SLOT 1 OCCUPIED");
    else
        snprintf(line1, sizeof(line1), "SLOT 2 OCCUPIED");

    /* Keep the duration compact so it fits on the LCD row. */
    snprintf(line2, sizeof(line2),
             "%02lu:%02lu:%02lu",
             (unsigned long)hours,
             (unsigned long)minutes,
             (unsigned long)seconds);

    lcd_write_line(0, line1);
    lcd_write_line(1, line2);
}


int main(void)
{
    uint8_t occupied1;
    uint8_t occupied2;

    uint16_t distance = 0;
    distance_state_t state = SAFE;
    uint32_t ultra_refresh_ms = 0;

    /* Initialize the existing timer driver. */
    timer_init();

    /* Configure both IR inputs with pull-ups. */
    gpio_mode('D', IR1_PIN, GPIO_INPUT_PULLUP);
    gpio_mode('D', IR2_PIN, GPIO_INPUT_PULLUP);

    /* Configure the LCD mode button. */
    gpio_mode(BUTTON_PORT, BUTTON_PIN, GPIO_INPUT_PULLUP);

    /* Configure status LEDs. */
    gpio_mode(LED_PORT, LED_GREEN, GPIO_OUTPUT);
    gpio_mode(LED_PORT, LED_YELLOW, GPIO_OUTPUT);
    gpio_mode(LED_PORT, LED_RED, GPIO_OUTPUT);

    /* Initialize peripherals. */
    ir_init();
    ultra_init();
    lcd_init();
    pwm_init();
    pwm_mute();

    previous_raw = gpio_read(BUTTON_PORT, BUTTON_PIN);
    stable_button = previous_raw;

    lcd_write_line(0, "SMART PARKING");
    lcd_write_line(1, "SYSTEM READY");

    ms_delay(500);

    /* Initial distance measurement */
    distance = get_filtered_distance();
    state = get_distance_state(distance, state);

    while (1)
    {
        /* Read parking-slot occupancy. */
        occupied1 = slot_occupied(IR1_PIN);
        occupied2 = slot_occupied(IR2_PIN);

        /* When Slot 1 detects the car, reversing assist completes and everything stops */
        if (occupied1)
        {
            state = STOP;
            distance = 0;
            update_leds(STOP);
            if (buzzer_on)
            {
                pwm_mute();
                buzzer_on = 0;
                active_frequency = 0;
            }
        }
        else
        {
            /* Measure reversing distance periodically (HC-SR04 requires >= 60ms between pings). */
            if ((uint32_t)(elapsed_ms - ultra_refresh_ms) >= 80)
            {
                ultra_refresh_ms = elapsed_ms;
                distance = get_filtered_distance();
                state = get_distance_state(distance, state);

                /* Compensate elapsed_ms for blocking ultrasonic measurement */
                if (distance == 0)
                    elapsed_ms += 30;
                else
                    elapsed_ms += (uint32_t)distance * 58 / 1000;
            }

            /* Normal reversing assist */
            update_leds(state);
            update_buzzer(state, distance);
        }

        /* Keep duration tracking and mode button active */
        update_slot_duration(occupied1, occupied2);
        update_button();

        /* Refresh the LCD periodically. */
        if ((uint32_t)(elapsed_ms - lcd_refresh_ms) >= 250)
        {
            lcd_refresh_ms = elapsed_ms;

            if (lcd_mode == MODE_SLOTS)
            {
                show_slot_mode(occupied1, occupied2);
            }
            else if (lcd_mode == MODE_DISTANCE)
            {
                show_distance_mode(distance, state, occupied1);
            }
            else
            {
                show_duration_mode(occupied1, occupied2);
            }
        }

        ms_delay(LOOP_DELAY_MS);
        elapsed_ms += LOOP_DELAY_MS;
    }
}

