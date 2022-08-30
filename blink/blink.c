// A simple sanity check. Toggles all pins on portb at 1 hz.
// 
// Flash fails:
//   Incorrect Target, Incorrect power, other components on RST/MOSI/MISO/SCK
// No blink:
//   Incorrent power, wrong pins
// Wrong rate:
//   F_CPU is set wrong for your chip. (Try passing an argument to make or editing Makefile)

#include <util/delay.h>
#include <avr/io.h>

int main() {
	DDRB = 0xFF;
	while (1) {
		_delay_ms(500);
		PORTB = 0xFF;
		_delay_ms(500);
		PORTB = 0x00;
	}
	return 0;
}


