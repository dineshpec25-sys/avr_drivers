MCU = atmega2560
PORT = /dev/ttyACM0
BAUD = 115200

TARGET = keypad_test

CFLAGS = -mmcu=$(MCU) -Os -Wall -I../../include -I../../hm

LIB = ../../lib/libavr_drivers.a

DRIVER_SRC = $(wildcard ../../src/*.c)
DRIVER_OBJ = $(patsubst ../../src/%.c,../../build/%.o,$(DRIVER_SRC))

all: $(TARGET).hex

# Compile individual driver sources
../../build/%.o: ../../src/%.c
	mkdir -p ../../build
	avr-gcc $(CFLAGS) -c $< -o $@

# Rebuild the static library
.PHONY: FORCE
FORCE:

$(LIB): FORCE $(DRIVER_OBJ)
	mkdir -p ../../lib
	rm -f $@
	avr-ar rcs $@ $(DRIVER_OBJ)

# Build the application
$(TARGET).elf: $(TARGET).c $(LIB)
	avr-gcc $(CFLAGS) -o $@ $< $(LIB)

# Generate the HEX file
$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size $<

# Flash the ATmega2560
flash: $(TARGET).hex
	avrdude -p $(MCU) -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:$<:i

# Clean application outputs
clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean
