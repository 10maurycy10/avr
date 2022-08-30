# A template makefile for avr development

TARGET?=atmega328p
SRC=template.c
AVRDUDETARGET?=m328p
PROGRAMER?=avrisp2
F_CPU?=1000000

.DEFAULT_GOAL := all.elf

all.elf : $(SRC)
	avr-gcc -o all.elf $(SRC) -mmcu=$(TARGET) -Os -Wall -DF_CPU=$(F_CPU)

clean:
	rm all.elf

flash: all.elf
	avrdude -c $(PROGRAMER) -p $(AVRDUDETARGET) -U flash:w:all.elf -e
