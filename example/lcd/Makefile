MCU = atmega2560
PORT = /dev/ttyACM0
BAUD = 115200
F_CPU = 16000000UL
TARGET = lcd_test

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra \
         -I../include -I../hm

LIB = ../../lib/libavr_drivers.a

.PHONY: all flash clean

all: $(TARGET).hex

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
