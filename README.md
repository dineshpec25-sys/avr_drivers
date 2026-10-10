# avr_drivers

Bare-metal drivers for the **ATmega2560** (Arduino Mega 2560 board): GPIO, timers, PWM and ADC, plus the peripherals built on top of them (LCD, keypad, ultrasonic sensor, IR sensor, 7-segment display and buzzer).

Everything talks to the hardware through registers. There is no Arduino core, and the driver code uses no `avr/io.h` or other avr-libc headers. Register addresses are written out in this repository and checked against the ATmega2560 datasheet (included in `Data Sheets/`).

**Target:** ATmega2560 at 16 MHz (Arduino Mega 2560)

## Contents

- [Drivers](#drivers)
- [Default pin map](#default-pin-map)
- [Repository layout](#repository-layout)
- [Getting started](#getting-started)
- [Using the drivers in your own project](#using-the-drivers-in-your-own-project)
- [API reference](#api-reference)
- [Examples](#examples)
- [Smart parking demo](#smart-parking-demo)
- [How the code is organized](#how-the-code-is-organized)
- [Datasheets](#datasheets)

## Drivers

| Driver | Header | What it does | Hardware it uses |
|---|---|---|---|
| GPIO | `gpio.h` | Set the mode of, write and read any pin on ports A to L | All ports |
| Timer | `timer.h` | Millisecond delays, and microsecond-resolution time measurement | Timer0 (1 ms tick), Timer1 (measurement) |
| PWM | `pwm.h` | PWM duty cycle and buzzer tones | Timer2, output OC2A on pin 10 |
| ADC | `adc.h` | 10-bit analog reads (blocking) | ADC0 to ADC7 |
| LED | `led.h` | On, off and toggle for one LED | PB7 (pin 13, onboard LED) |
| Keypad | `keypad.h` | Scan a 4x4 matrix keypad | PORTA |
| LCD | `lcd.h` | 16x2 HD44780 display in 4-bit mode | PORTL |
| Ultrasonic | `ultra.h` | Distance in cm from an HC-SR04 style sensor | PJ0, PJ1 and Timer1 |
| IR | `ir.h` | Slot occupied or available from an IR sensor | PD2 by default |
| 7-segment | `seg7.h` | Show one digit, 0 to 9, on pins you choose | Any pins |

## Default pin map

| Peripheral | Port bits | Mega pins |
|---|---|---|
| Onboard LED | PB7 | 13 |
| IR sensor (default) | PD2 | 19 |
| Keypad rows (outputs) | PA0 to PA3 | 22 to 25 |
| Keypad columns (inputs, pull-ups) | PA4 to PA7 | 26 to 29 |
| LCD RS and EN | PL0 and PL1 | 49 and 48 |
| LCD D4 to D7 | PL2 to PL5 | 47 to 44 |
| Ultrasonic TRIG and ECHO | PJ0 and PJ1 | 15 and 14 |
| Buzzer / PWM output | PB4 (OC2A) | 10 |
| ADC inputs | ADC0 to ADC7 | A0 to A7 |
| 7-segment display | set with `seg7_init()` | your choice |

To change a default, edit the matching file in `hm/` (`led_regs.h`, `ir_regs.h`, `keypad_regs.h`) or the pin macros in `include/lcd.h` and `include/ultra.h`.

### LCD wiring (16-pin HD44780 module)

| LCD pin | Name | Connect to |
|---|---|---|
| 1 | VSS | GND |
| 2 | VDD | 5 V |
| 3 | V0 | middle pin of a contrast potentiometer |
| 4 | RS | Mega pin 49 |
| 5 | RW | GND |
| 6 | E | Mega pin 48 |
| 7 to 10 | D0 to D3 | not connected |
| 11 to 14 | D4 to D7 | Mega pins 47, 46, 45, 44 |
| 15 | A (backlight +) | 5 V, through a resistor if the module has none |
| 16 | K (backlight -) | GND |

## Repository layout

```
avr_drivers/
├── include/        public headers, the ones applications include
├── src/            driver implementations (*_field.c, keypad.c)
├── hm/             hardware definitions: register addresses and default pins
├── lib/            libavr_drivers.a, the compiled driver library
├── example/        demo programs, each folder has its own Makefile
│   ├── main.c      smart parking demo, uses most of the drivers together
│   ├── Makefile    builds the library and main.c
│   ├── gpio_led_switch/
│   ├── timer/
│   ├── pwm/
│   ├── ultra/
│   ├── ir/         digital/ and analog/
│   ├── keypad/
│   └── 7seg/
├── Data Sheets/    datasheets for the chip and the parts
├── reference.txt   useful links
└── README.md
```

## Getting started

### What you need

- An Arduino Mega 2560 (or any ATmega2560 board with the Arduino bootloader) and a USB cable
- `avr-gcc`, `avr-binutils`, `avr-libc`, `avrdude` and `make`

`avr-libc` is still needed at link time because it provides the startup code that runs before `main`. The driver code does not include any of its headers.

```bash
# Debian / Ubuntu
sudo apt install gcc-avr binutils-avr avr-libc avrdude make

# Arch
sudo pacman -S avr-gcc avr-binutils avr-libc avrdude make
```

Give your user access to the serial port, then log out and back in:

```bash
sudo usermod -aG dialout $USER     # on Arch the group is uucp
```

### Build and flash the demo

```bash
git clone https://github.com/dineshpec25-sys/avr_drivers.git
cd avr_drivers/example
make
make flash
```

- `make` compiles every file in `src/`, packs them into `lib/libavr_drivers.a`, and builds `main.hex`.
- `make flash` uploads it with avrdude.
- The default port is `/dev/ttyACM0`. Check yours with `ls /dev/ttyACM* /dev/ttyUSB*`, then run for example `make flash PORT=/dev/ttyUSB0`.
- `make clean` removes the generated `.elf` and `.hex` files.

### Build one of the other examples

```bash
cd example/ultra
make
make flash
```

Each example Makefile links against `lib/libavr_drivers.a`. If you change anything in `src/`, run `make` in `example/` first so the library is rebuilt.

## Using the drivers in your own project

Compile your program against the headers and the library:

```bash
avr-gcc -mmcu=atmega2560 -Os -I path/to/avr_drivers/include \
        -o app.elf app.c path/to/avr_drivers/lib/libavr_drivers.a
avr-objcopy -O ihex -R .eeprom app.elf app.hex
```

A complete program that blinks the onboard LED:

```c
#include "gpio.h"
#include "timer.h"

int main(void)
{
    gpio_mode('B', 7, GPIO_OUTPUT);     /* onboard LED, pin 13 */
    timer_init();

    while (1)
    {
        gpio_write('B', 7, HIGH);
        ms_delay(500);
        gpio_write('B', 7, LOW);
        ms_delay(500);
    }
}
```

## API reference

### GPIO (`gpio.h`)

```c
void         gpio_mode (char PORT, uint8_t PIN, gpio_mode_t MODE);
void         gpio_write(char PORT, uint8_t PIN, gpio_write_t value);
gpio_write_t gpio_read (char PORT, uint8_t PIN);
```

- `PORT` is a letter from `'A'` to `'L'`. There is no port I on the ATmega2560. `PIN` is 0 to 7.
- Modes: `GPIO_INPUT`, `GPIO_OUTPUT`, `GPIO_INPUT_PULLUP`.
- Values: `HIGH`, `LOW`. Always use these names, not plain 0 or 1.

### Timer (`timer.h`)

```c
void     timer_init(void);
void     ms_delay(uint16_t ms);

void     timer_measure_reset(void);
void     timer_measure_start(void);
void     timer_measure_stop(void);
uint32_t timer_measure_get(void);
```

- `timer_init()` must be called first. It sets up Timer0 as a 1 ms tick and prepares Timer1.
- `ms_delay()` blocks for the given number of milliseconds, using Timer0.
- `timer_measure_*` use Timer1 as a stopwatch. One count is 0.5 us (16 MHz divided by 8).

### PWM and buzzer (`pwm.h`)

```c
void pwm_init(void);
void pwm_start(void);
void pwm_stop(void);
void pwm_set_duty(uint8_t duty);        /* 0 to 100 percent */
void pwm_tone(uint16_t frequency_hz);
void pwm_mute(void);
```

- Output is on **pin 10** (PB4, OC2A), driven by Timer2.
- `pwm_init()` configures Fast PWM at 50% duty with the timer stopped. `pwm_start()` starts it with a prescaler of 64, which gives about 977 Hz.
- `pwm_tone()` produces a square wave of the requested frequency, which is what a buzzer needs. It works from about 31 Hz upward. `pwm_mute()` stops the timer and disconnects the pin.
- PWM mode (`pwm_set_duty`) and tone mode (`pwm_tone`) both use Timer2, so use one or the other at a time.

### ADC (`adc.h`)

```c
void     adc_init(void);
uint16_t adc_read(uint8_t channel);     /* returns 0 to 1023 */
```

- Reference is AVCC (5 V), the ADC clock is 125 kHz (prescaler 128), and the result is right-adjusted.
- `adc_read()` blocks until the conversion is finished. Use channels 0 to 7 (pins A0 to A7).

### LED (`led.h`)

```c
void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
```

### Keypad (`keypad.h`)

```c
void keypad_init(void);
char keypad_get_key(void);
```

- 4x4 matrix with the layout `1 2 3 A / 4 5 6 B / 7 8 9 C / * 0 # D`.
- `keypad_get_key()` returns the key character, or `'\0'` when nothing is pressed. It waits for the key to be released before it returns.

### LCD (`lcd.h`)

```c
void lcd_init(void);
void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void lcd_string(const char *str);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print_number(uint16_t number);
```

- 16x2 HD44780 in 4-bit mode on PORTL (see the wiring table above).
- `row` is 0 or 1, `col` is 0 to 15.

### Ultrasonic sensor (`ultra.h`)

```c
void     ultra_init(void);
void     ultra_trigger(void);
uint32_t ultra_get_echo_time_us(void);
uint16_t ultra_get_distance_cm(void);
```

- TRIG is PJ0 (pin 15) and ECHO is PJ1 (pin 14). The sensor needs 5 V.
- Distance is the echo time in microseconds divided by 58.
- Both measuring functions return **0** if no echo arrives within 30 ms, which means nothing is in range (about 5 m) or the sensor is not connected.
- Call `timer_init()` and `ultra_init()` before measuring. Leave at least 60 ms between measurements.

### IR sensor (`ir.h`)

```c
void          ir_init(void);
slot_status_t ir_get_status(void);
slot_status_t ir_get_status_pin(uint8_t pin);   /* any pin of the IR port */
```

- Returns `OCCUPIED` or `AVAILABLE`. The sensor is treated as active-low: the line goes low when an object is detected. Pull-ups are enabled.

### 7-segment display (`seg7.h`)

```c
void seg7_init(seg7_map_t map);
void seg7_clear(void);
void seg7_display(uint8_t num);          /* 0 to 9 */
```

- `seg7_map_t` holds the port and pin of each segment, `a` to `g`. Segments are driven HIGH to light, so use a common-cathode display.

## Examples

| Folder | What it does |
|---|---|
| `example/main.c` | Smart parking demo, see below |
| `example/gpio_led_switch/` | An LED follows a switch (LED on PE5, switch on PH6 with pull-up) |
| `example/timer/` | Blinks an LED on PE5 using `ms_delay()` |
| `example/pwm/` | Plays buzzer tones with `pwm_tone()` |
| `example/ultra/` | Lights an LED on PE5 when something is within 50 cm |
| `example/ir/digital/` | Lights an LED on PE5 when the IR sensor sees an object |
| `example/ir/analog/` | Reads an analog IR sensor on ADC0 and sets PWM brightness from it |
| `example/keypad/` | Shows the pressed digit in binary on four LEDs (PH6, PH5, PH4, PH3) |
| `example/7seg/` | Placeholder for a 7-segment test |

## Smart parking demo

`example/main.c` combines most of the drivers in one program. A reversing aid watches the distance behind a car, two IR sensors report which parking slots are taken, and an LCD shows the status on three pages.

### Wiring

| Part | Connection | Mega pin |
|---|---|---|
| IR sensor, slot 1 | PD2 | 19 |
| IR sensor, slot 2 | PD3 | 18 |
| Ultrasonic TRIG / ECHO | PJ0 / PJ1 | 15 / 14 |
| LCD | PORTL, as in the LCD wiring table | 49 to 44 |
| Page button (to GND, pull-up) | PE4 | 2 |
| Green, yellow, red LED | PG0, PG1, PG2 | 41, 40, 39 |
| Buzzer | PB4 | 10 |

### Behaviour

| Distance | LED | Buzzer |
|---|---|---|
| Nothing in range | green | silent |
| Over about 30 cm | green | slow short beep |
| About 16 to 30 cm | yellow | beep every half second |
| About 6 to 15 cm | red | fast beeps |
| About 5 cm or less | red | continuous tone |

The limits have a few centimeters of hysteresis so the LED does not flicker at a boundary, and the distance is median-filtered over three readings.

When slot 1 becomes occupied, the reversing aid stops and the buzzer goes quiet.

The button on PE4 cycles the LCD through three pages:

1. **Slots:** which of the two slots are free, and which one to use
2. **Distance:** the distance in cm and the warning state
3. **Duration:** how long each occupied slot has been taken

## How the code is organized

```
example / your program  ->  include/*.h  ->  src/*.c  ->  hm/*.h
```

- **`include/`** is the public interface. Applications include only these headers.
- **`src/`** holds the implementations. They build into `lib/libavr_drivers.a`.
- **`hm/`** is the only place that knows register addresses and default pins. Moving to another pin means editing one file here.
- Registers are accessed through `volatile` pointers to fixed addresses, with no vendor headers.
- The drivers poll and block. They do not use interrupts.
- Each driver owns its hardware: Timer0 is the millisecond tick, Timer1 is the measurement stopwatch, and Timer2 is the PWM and buzzer. They do not share a timer, so they can all run in the same program.

## Datasheets

The `Data Sheets/` folder contains:

- [ATmega2560](Data%20Sheets/ATmega2560%20Datasheet.pdf)
- [LCD](Data%20Sheets/LCD%20Datasheet.pdf)
- [Ultrasonic sensor](Data%20Sheets/Ultrasonic%20Datasheet.pdf)
- [IR sensor](Data%20Sheets/IR%20Sensor%20Datasheet.pdf)
- [Keypad](Data%20Sheets/Keypad%20Datasheeet.pdf)
- [7-segment display](Data%20Sheets/7-Segment%20Datasheet.pdf)
- [Buzzer](Data%20Sheets/Buzzer%20Datasheet.pdf)

## Author

Maintained by [@dineshpec25-sys](https://github.com/dineshpec25-sys).
