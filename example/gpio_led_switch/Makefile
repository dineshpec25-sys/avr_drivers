MCU = atmega2560
PORT = /dev/ttyACM0
BAUD = 115200

TARGET = gpio_led_switch

CFLAGS = -mmcu=$(MCU) -Os -Wall -I../include -I../hm
LDFLAGS = -L../lib -lavr_drivers

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	avr-gcc $(CFLAGS) -o $@ $< $(LDFLAGS)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size $<

flash: $(TARGET).hex
	avrdude -p $(MCU) -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:$<:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean
