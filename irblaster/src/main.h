// The data structues used to store the codes for main.c

#include <avr/pgmspace.h>
#include "ir.h"

// Data structure for holding codes in program memory
enum {
	CODE_NEC, // NEC
	CODE_NECEXT, // NECext
	CODE_RC5, // RC-5 and RC-5x
	CODE_SAM32, // Samsung32
	CODE_SIRC, CODE_SIRC15, CODE_SIRC20, // Sony infared, have 5, 15, and 20 command bits respecivly
};


union Code {
	uint8_t type;
	struct {
		uint8_t type;
		uint16_t address;
		uint16_t command;
	} necext;
	struct {
		uint8_t type;
		uint8_t address;
		uint8_t command;
	} rc5;
	struct {
		uint8_t type;
		uint8_t address;
		uint8_t command;
	} nec;
	struct {
		uint8_t type;
		uint16_t address;
		uint16_t command;
	} sam32;
	struct {
		uint8_t type;
		uint16_t address;
		uint8_t command;
	} sirc;
};
