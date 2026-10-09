MCU = atmega2560
PORT = /dev/ttyACM0
BAUD = 115200
TARGET = main
F_CPU = 16000000UL

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra \
         -I../include -I../hm

LIB = ../lib/libavr_drivers.a

DRIVER_SRC = $(wildcard ../src/*.c)
DRIVER_OBJ = $(patsubst ../src/%.c,../build/%.o,$(DRIVER_SRC))
DRIVER_HEADERS = $(wildcard ../include/*.h ../hm/*.h)

.PHONY: all clean flash FORCE

all: $(TARGET).hex

../build/%.o: ../src/%.c $(DRIVER_HEADERS)
	mkdir -p ../build
	avr-gcc $(CFLAGS) -c $< -o $@

FORCE:

$(LIB): FORCE $(DRIVER_OBJ)
	mkdir -p ../lib
	rm -f $@
	avr-ar rcs $@ $(DRIVER_OBJ)

$(TARGET).elf: $(TARGET).c $(LIB)
	avr-gcc $(CFLAGS) -o $@ $< $(LIB)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size $<

flash: $(TARGET).hex
	avrdude -p $(MCU) -c wiring -P $(PORT) -b $(BAUD) \
	        -D -U flash:w:$<:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex
