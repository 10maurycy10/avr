# AVR

Some random avr microcontroler code.

## Building

First ``cd`` into a subdirectory and run ``make``.

If you are not building for the ATMEGA328P microcontroler, specify ``TARGET`` to make.

You can find all MCUs by running ``avr-gcc --target-help``

If the clock speed of the microcontroler is not 1MHz, specify ``F_CPU`` to the make.

### Build system

The makefiles in projects call the ``Makefile`` at the root of the repo. If you wish to build out of tree, it must be inlined.

## Flashing

``make flash``

You may have to run it as root.

This will preform an erase cycle

If you are using a different programmer than the avrisp2, specify ``PROGRAMMER`` to make.

### Preserving EEPROM

``make flashkeep``

This backs up the EEPROM to dump.hex, erases and program the chip, and restores EEPROM.

If you plan on doing this often, setting the EESAVE fuse is a better option.

## Requirements

- avr-gcc tool chain

- avrdude programmer controller (if you want to use ``make flash``)

You probably want the actual hardware (an avr microcontroler and programmer)

On Arch linux and Arch based distros, these can be installed like this:

```
pacman -S avr-gcc avr-libc avrdude
```

On Debian and Debian based distros:

```
apt install gcc-avr avr-libc avrdude
```
