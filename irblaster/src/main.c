// Transmits evry stored code

#include "main.h"
#include "../codedb.h"

void main() {
	initTimer0();
	sei();
	
	// Wait a bit to let capacitors charge
	_delay_ms(200);
	
//	while (1) {
//		transmit_sirc(0x5, 0xff, 5);
//		_delay_ms(20);
//	}
	

	// Test code
	transmit_nec(0x01, 0x18);

	for (int i = 0; i < numcodes; i++) {
		uint8_t type = pgm_read_byte(&codes[i].type);
		// NEC and varients
		if (type == CODE_NEC) {
			uint8_t address = pgm_read_byte(&codes[i].nec.address);
			uint8_t command = pgm_read_byte(&codes[i].nec.command);
			transmit_nec(address, command);
		}
		if (type == CODE_NECEXT) {
			uint16_t address = pgm_read_word(&codes[i].necext.address);
			uint16_t command = pgm_read_word(&codes[i].necext.command);
			transmit_necext(address, command);
		}
		// RC5 and varients
		if (type == CODE_RC5) {
			uint8_t address = pgm_read_byte(&codes[i].rc5.address);
			uint8_t command = pgm_read_byte(&codes[i].rc5.command);
			transmit_rc5(address, command);
		}
		// Samsung
		if (type == CODE_SAM32) {
			uint16_t address = pgm_read_word(&codes[i].sam32.address);
			uint16_t command = pgm_read_word(&codes[i].sam32.command);
			transmit_samsung32(address, command);
		}
		// SIRC and varients
		if (type == CODE_SIRC) {
			uint16_t address = pgm_read_word(&codes[i].sirc.address);
			uint8_t command = pgm_read_byte(&codes[i].sirc.command);
			transmit_sirc(address, command, 5);
		}
		if (type == CODE_SIRC15) {
			uint16_t address = pgm_read_word(&codes[i].sirc.address);
			uint8_t command = pgm_read_byte(&codes[i].sirc.command);
			transmit_sirc(address, command, 8);
		}
		if (type == CODE_SIRC20) {
			uint16_t address = pgm_read_word(&codes[i].sirc.address);
			uint8_t command = pgm_read_byte(&codes[i].sirc.command);
			transmit_sirc(address, command, 13);
		}
		_delay_ms(10);
	}
	
	while (1) {;}
}

