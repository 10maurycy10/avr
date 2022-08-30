// A random number generator using two 7 segment displays and 2 buttons
// Can simulate a coin toss, d4, standard d6, d8, d12, d20, d100.

#include <avr/io.h>
#include <util/delay.h>

// The displays have a common anode configuration
// Each display has a single positive pin, and 7 negatives (one for each segment)
// To display a multi-digit number, Each display is quickly flashed, too fast to notice.
//
// PortB:
//   0-6: Display negative pines
// PortD:
//   0: Display 1 positive
//   1: Display 2 positive
// PortC:
//   0: "Roll" button
//   1: Mode select button
//

// Friendly names for registers
#define DISPLAYPORT PORTB
#define DISPLAYDDR  DDRB

#define INPIN PINC
#define INDDR  DDRC

#define CATHODEDDR DDRD
#define CATHODEPORT PORTD

// Shortcuts for checking if a button is pressed
#define BUTTON (INPIN & 0b1)
#define MODE (INPIN & 0b10)
#define NOBUTTON (!MODE && !BUTTON)

// All modes, each simulating different sided dice.
int mode_count = 7;
int modes[] = {
	2,
	4,
	6,
	8,
	12,
	20,
	100,
};

// Table of digits.
// Bits 0-6 are display cerements. (Left to right, top to bottom)
uint8_t digits[10] = {
	0b01110111, //0
	0b00100100, //1
	0b01011101, //2
	0b01101101, //3
	0b00101110, //4
	0b01101011, //5 
	0b01111011, //6
	0b00100101, //7
	0b01111111, //8
	0b01101111, //9
};

// Display a number between 0 and 99 (inclusive), additional digits are truncated.
// Must be called every few ms
void print_number(int number) {
	// Break number into digits
	int d1 = (number / 10) % 10;
	int d2 = (number / 1 ) % 10;
	// Display first digeit for 5 ms
	DISPLAYPORT = ~digits[d1];
	CATHODEPORT = 0b01;
	_delay_ms(5);
	// Display second digit for 5 ms
	DISPLAYPORT = ~digits[d2];
	CATHODEPORT = 0b10;
	_delay_ms(5);
	// Blank display
	CATHODEPORT = 0b00;
}

// Program exectuion starts here:
int main() {
	// Onetime register initalization
	CATHODEDDR = 0b11;
	DISPLAYDDR = 0xff;
	INDDR = 0x00;
	// rapedly incremented and used to time keystrokes as a source of randomness
	int counter = 0;
	// Keep track of the max number
	int modeid = 3;
	int modulus = modes[modeid];
	while (1) {
		// If the mode button is pressed, cycle to the next mode...
		if (MODE) {
			modeid ++;
			modeid = modeid % mode_count;
			modulus	= modes[modeid];
			// Display mode as long as button is pressed.
			while (MODE) {
				print_number(modulus);
			}
			// Keep display on for 1000 ms after button is released as long as no other button is pressed.
			_delay_ms(20); // Delay for debouncing
			for (int i = 0; i < 100 && NOBUTTON; i++) {
				print_number(modulus);
			}
		}
		// If the roll button is pressed, save counter and display number
		if (BUTTON) {
			// Add one as dice go from 1-Max Inclusive, but the counter goes from 0-Max exclusive.
			int number = counter + 1;
			// Display number while button is pressed
			while (BUTTON)
				print_number(number);
			// Keep display om for 1000 ms as long as no button is pressed, incrementing counter during this time.
			_delay_ms(20); // Delay for debouncing
			for (int i = 0; i < 100 && NOBUTTON; i++) {
				print_number(number);
				counter++;
				counter = counter%modulus;
			}
		// If nothing is happening, increment counter
		} else {
			counter++;
			counter = counter%modulus;
		}
	}
	return 0; // keep compilers quite
}

