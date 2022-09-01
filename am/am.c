// A super simple AM transmitter, requiring an avr and a single PNP transistor.
//
// The microcontroler must be clocked at 1 MHz (or your desired transmit frequency), and have clock output enabled
// For an atmega328p (with an 8MHz internal oscillator), setting the lfuse byte to 0x22 is fine.
//
// Your finger of a long wire (100cm to 1m) can serve as an antenna.
//
// (Avrdude: -U lfuse:w:0x22:m)
//
// External circuit:
//
// PB0/CLKO-----+
//              |
//           | /
// PD7 ------|(
//           | V
//              \
//               +---- To antenna
//               |
//               +-\/\/\/\--- GND
//                  optional resistor (300ohms) (increases modulation depth)
//
// Don't use this for a high power transmission, as the square wave output it 
// creates has a lot of harmonics, which will cause a lot of interference on other frequencies.

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define NPS 4
#define BEEP_LENGTH (1000/NPS)

// Declared as volatile to stop the compiler from thinking these are unnecessary.
volatile int cycles = 500; // Cycles between toggles
volatile int on = 0;

// Timer over flow interrupt, called when timer reaches 65536 (2**16)
ISR (TIMER0_OVF_vect) {
	// If playing sound, toggle pin
	if (on)
		PORTD ^= (1 << 7);
	// set timer back for next toggle	
	TCNT0 = 65536 - (cycles/4);
}

// Play a tone
void beep(int freq, int ms) {
	int delay = (1000000 / freq) / 2; // Time wait between toggles, in us
	cycles = delay * (F_CPU / 1000000); // Time to wait, in cycles
	on = 1;
	for (int i = 0; i < ms; i++)
		_delay_ms(1);
	on = 0;
}

void note(int freq) {
	beep(freq, BEEP_LENGTH);
}


int main() {
	// Set outputs
	DDRB=0xFF;
	DDRD=0xff;
	// Setup timer,  prescaler tp 4
	TCCR0B = 1;
	TCNT0 = 0;
	TIMSK0 |= (1 << TOIE1);
	sei();
	int counter = 0;
	while (1) {
		if (counter % 2 == 0) {
			if (counter % 5 == 0) {
				note(600);
			} else if (counter % 4 == 0) {
		 		note(700);
			} else {
				note(500);
			}
		} else {
			note(600);
		}
		counter ++;
	}
	return 0;
	
}
