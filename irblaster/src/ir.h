// IR remote control transmiter library.
//
// Supported protocols:
// - NEC
// - NECext
// - RC5
// - RC5x
// - Samsung32
//
// It depends on timer0, it will have to be modified if you are already using it.
//
// Before using this library to transmit anything, call initTimer0() and sei().
// If it's not being used, disabling interupts or stoping hardware timer 0 will prevent the avr from constatnly waking up to generate a carrier.

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Timing calibration procedure:
//	use dead_carrier to set the cycles count to one of the constants, adust the constant untill the freqency is correct
// 	Finaly adjust loops per 10 us for the right burst timeing
#define TIMER_CYCLES_36 80 // 36 KHz
#define TIMER_CYCLES_38 75 // 38 Khz
#define TIMER_CYCLES_40 68 // 40 Khz
#define LOOPS_PER_10_US 12

// The connection point of the IR LED or driver.
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_BIT 0b00000110

void initTimer0(); // Initialize the timer and set the LED's pin to output
ISR (TIMER0_OVF_vect);

// Set the freqency used by transmit_element, the argument should be one of the TIMER_CYCLES_* macros
void set_freqency(uint16_t c); 
// Transmit a pulse of ontime*10 us wait offtime*10 us.
void transmit_element(uint16_t ontime, uint16_t offtime);
// Sets the timer to continualy generate a carrer, can be turned off by calling transmit_element(0,0)
// the argument should be one of the TIMER_CYCLES_* constants.
// This is intended for callubration, not data transmision
void dead_carrier(uint16_t c); 

// High level transmit functions
void transmit_nec(uint8_t address, uint8_t command);
void transmit_necext(uint16_t address, uint16_t command);
void transmit_rc5(uint8_t address, uint8_t command); // Also works for RC5x
void transmit_samsung32(uint16_t address, uint16_t command);
void transmit_sirc(uint16_t address, uint8_t command, uint8_t address_bits); // Varrients of SIRC
void transmit_kaseikyo(uint32_t address, uint16_t command); // Kaseiyko, see ir.c for information on the address field
void transmit_rc6(uint16_t address, uint8_t command); // RC-6
