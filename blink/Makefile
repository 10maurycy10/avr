TARGET?=atmega328p
SRC=blink.c
PROGRAMER?=avrisp2
F_CPU?=1000000

.DEFAULT_GOAL := default

default: all.elf size

all.elf : $(SRC)
	avr-gcc -o all.elf $(SRC) -mmcu=$(TARGET) -Os -Wall -DF_CPU=$(F_CPU)

clean:
	rm all.elf

flash: all.elf
	avrdude -c $(PROGRAMER) -p $(TARGET) -U flash:w:all.elf -e

size: all.elf
	avr-size --mcu=$(TARGET) all.elf -C

flashkeep: all.elf
	avrdude -c $(PROGRAMER) -p $(TARGET) -U flash:r:dump.hex
	avrdude -c $(PROGRAMER) -p $(TARGET) -U flash:w:all.elf -U eeprom:w:dump.hex
