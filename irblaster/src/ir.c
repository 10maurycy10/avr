#include "ir.h"

////////////////////////////////////////////////////////////
// Hardware timer 0 is usesd to create the carrier signal //
////////////////////////////////////////////////////////////

volatile int carrier = 0; // If this is one, a carrier will be output
volatile int cycles = TIMER_CYCLES_36;

void initTimer0() {
	// Set the LED's ddr
	LED_DDR |= LED_BIT;
        // Setup timer,  prescaler to x1
        TCCR0B = 0b01;
        TCNT0 = 0;
        TIMSK0 |= (1 << TOIE1);
}

// Hardware timer handler:
ISR (TIMER0_OVF_vect) {
        // If playing sound, toggle pin
        if (carrier) {
                LED_PORT ^= LED_BIT;
	} else {
                LED_PORT &= ~LED_BIT;
	}
        // set timer back for next toggle
        TCNT0 = (255 - (cycles));
}

///////////////////////////////
// IR transmisison functions //
///////////////////////////////

// Protocol reference:
// https://www.mikrocontroller.net/articles/IRMP_-_english

// LSB = 0
#define GET_BIT(value, bit) ((value) & (1<<(bit)))

// TODO Use timer interupts here instead to get better acuracy and freqency independece.
// Transmit a pulse of ontime*10 us wait offtime*10 us.
void transmit_element(int ontime, int offtime) {
	if (ontime) {
		carrier = 1;
		 _delay_loop_2(ontime * LOOPS_PER_10_US);
	}
	carrier = 0;
	if (offtime) {
		_delay_loop_2(offtime * LOOPS_PER_10_US);
	}
}

void nec_send_bit(uint16_t value) {
	if (value) {
		transmit_element(56, 56);
	} else {
		transmit_element(56, 168);
	}
}


// High level transmit function for NEC codes
void transmit_nec(uint8_t address, uint8_t command) {
	cycles = TIMER_CYCLES_36;
	void send_byte(uint8_t value) {
		// NEC is LSB first.
		for (int i = 0; i < 8; i++) nec_send_bit(GET_BIT(value, i));
	}
	
	transmit_element(900, 450); // NEC header
	// Address portion
	send_byte(address);
	send_byte(~address);
	// Command portion
	send_byte(command);
	send_byte(~command);
	// End of message burst
	transmit_element(56, 0);
}

void transmit_necext(uint16_t address, uint16_t command) {
	cycles = TIMER_CYCLES_36;
	void send_2_bytes(uint16_t value) {
		// NEC is LSB first.
		for (int i = 0; i < 16; i++) nec_send_bit(GET_BIT(value, i));
	}
	transmit_element(900, 450); // NEC header
	// Data
	send_2_bytes(address);
	send_2_bytes(command);
	// End of message burst
	transmit_element(56, 0);
}

// RC-5/RC-5x transmiter
void transmit_rc5(uint8_t address, uint8_t command) {
	cycles = TIMER_CYCLES_36;
	void send_bit(uint8_t value) {
		// RC-5 uses BPSK for sendig data:
		if (value) {
			transmit_element(0, 89);
			transmit_element(89, 0);
		} else {
			transmit_element(89, 0);
			transmit_element(0, 89);
		}
	}
	
	// Start bits, and 6th bit of the command for RC-5x
	send_bit(1);
	if (GET_BIT(command, 6)) {
		send_bit(0);
	} else {
		send_bit(1);
	}
	// Togle bit, TODO handle this better?
	send_bit(0);
	// Transmit the address, MSB first
	for (int i = 0; i < 6; i++) send_bit(GET_BIT(address, 5-i));
	// Transmit the rest of the command, MSB first
	for (int i = 0; i < 6; i++) send_bit(GET_BIT(command, 5-i));
}

void transmit_samsung32(uint16_t address, uint16_t command) {
	cycles = TIMER_CYCLES_38;
	void send_bit(uint16_t value) {
		if (value) {
			transmit_element(55, 165);
		} else {
			transmit_element(55, 55);
		}
	}
	void send_2_bytes(uint16_t value) {
		// Samsumg32 has the LSB first.
		for (int i = 0; i < 16; i++) send_bit(GET_BIT(value, i));
	}
	// Start burst
	transmit_element(450, 450);
	// address
	send_2_bytes(address);
	// Command
	send_2_bytes(command);
	// End bit
	transmit_element(55, 0);

}

// For SIRC, use 5 address bits, for SIRC15, use 7, and for SIRC 20 use 13
void transmit_sirc(uint16_t address, uint8_t command, uint8_t address_bits) { 
	// SIRC sends the LSB first
	void send_bit(uint32_t value) {
		if (value) {
			transmit_element(120, 60);
		} else {
			transmit_element(60, 60);
		}
	}
	void send_data(uint16_t value, uint8_t bits) {
		for (int i = 0; i < bits; i++) send_bit(GET_BIT(value, i));
	}
	// Start bit
	transmit_element(240, 60);
	send_data(command, 7);
	send_data(address, address_bits);
	_delay_ms(10); // Because SIRC recivers have to wait to detemine which protocol is being recived, insert a delay to avoid confusing it.
}

void dead_carrier(uint16_t c) {
	carrier = 1;
	cycles = c;
}
