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
void transmit_element(uint16_t ontime, uint16_t offtime) {
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
	for (int i = 0; i < 5; i++) send_bit(GET_BIT(address, 4-i));
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
	cycles = TIMER_CYCLES_40;
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

void transmit_rc6(uint16_t address, uint8_t command) { 
	cycles = TIMER_CYCLES_36;
	void send_bit(uint8_t value) {
		if (value) {
			transmit_element(44, 0);
			transmit_element(0, 44);
		} else {
			transmit_element(0, 44);
			transmit_element(44, 0);
		}
	}
	// Start bit
	transmit_element(266, 89);
	send_bit(1); 
	// Mode bits
	send_bit(0);
	send_bit(0);
	send_bit(0);
	// Togle bit
	send_bit(0);
	// Address, MSB first
	for (int i = 0; i < 8; i++) send_bit(GET_BIT(address, 7-i));
	// Data, MSB first
	for (int i = 0; i < 8; i++) send_bit(GET_BIT(command, 7-i));
	// Pause
	transmit_element(0, 266);
}

// https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/ir_Kaseikyo.hpp 
// The vendor ID, ID and Genre are packed into a signle address argument for compatibility with .ir files 
// LSB first.
// Format:
//		Start bit
// 	16	Vendor ID
//	4	Parity: XOR of very 4 bits of the vendor id
//	4	Genre1
//	4	Genre2
//	10 	Command
//	2 	Id
//	8 	Parity: Bytewize XOR of eveyr byte after the vendor ID
//
// Address arguemnt layout (MSB first):
//	2	Id
//	16	Vendor id
//	4	Genre1
//	4	Genre2
//
void transmit_kaseikyo(uint32_t address, uint16_t command) {
	cycles = TIMER_CYCLES_38;
	void send_bit(uint32_t bit) {
		if (bit) transmit_element(42, 126);
		if (!bit) transmit_element(42, 42);
	}
	// Transmision buffer, the message has to be stored to compute parity
	uint8_t buff[6];

	// Split the address into id, vendor and genre infrmation
	uint8_t id = (address >> 24) & 3;
	uint16_t vendor_id = (address >> 8) & 0xffff;
	uint8_t genre1 = (address >> 4) & 0xf;
	uint8_t genre2 = address & 0xf;
	// Add the vendor id to the buffer
	buff[0] = (uint8_t)(vendor_id & 0xff);
	buff[1] = (uint8_t)(vendor_id >> 8);
	// Compute the vendor parity, 4 bits
	uint8_t vendor_parity = buff[0] ^ buff[1];
	vendor_parity = (vendor_parity & 0xf) ^ (vendor_parity >> 4);
	// Add the party and the genre information
	buff[2] = (vendor_parity & 0xf) | (genre1 << 4);
	// Wirte the rest of the genre and the command
	buff[3] = (genre2 & 0xf) | ((uint8_t)(command & 0xf) << 4);
	buff[4] = (id << 6) | (uint8_t)(command >> 4);
	// Wirte the final parity byte
	buff[5] = buff[2] ^ buff[3] ^ buff[4];
	
	// Start bit
	transmit_element(338, 169);
	// Everything else
	for (int i = 0; i < sizeof(buff)/sizeof(uint8_t); i++) {
		for (int e = 0; e < 8; e++) {
			send_bit(GET_BIT(buff[i], e));
		}
	}
	// Stop bit
	transmit_element(42, 0);
}

void dead_carrier(uint16_t c) {
	carrier = 1;
	cycles = c;
}
