MCU = atmega2560
PORT = /dev/ttyACM0
BAUD = 115200

TARGET = timer_test

CFLAGS = -mmcu=$(MCU) -Os -Wall -I../include -I../hm

DRIVER_SRC = $(wildcard ../src/*.c)
DRIVER_OBJ = $(patsubst ../src/%.c,../build/%.o,$(DRIVER_SRC))
LIB = ../../lib/libavr_drivers.a

all: $(TARGET).hex

$(LIB): $(DRIVER_OBJ)
	mkdir -p ../lib
	avr-ar rcs $@ $^

../build/%.o: ../src/%.c
	mkdir -p ../build
	avr-gcc $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(TARGET).c $(LIB)
	avr-gcc $(CFLAGS) -o $@ $< $(LIB)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size $<

flash: $(TARGET).hex
	avrdude -p $(MCU) -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:$<:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex
	rm -f ../build/*.o
	rm -f ../lib/libavr_drivers.a

.PHONY: all flash clean
