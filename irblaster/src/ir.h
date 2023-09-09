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
// 	First adjust the TIMER_CYCLES_36 until the carrier is at 36 KHz, and TIMER_CYCLES_38 for 38 Khz
//	Then callibrate the LOOPS_PER_10_US, starting at 20 and going down to set the timing of the trasnmited data.

#define TIMER_CYCLES_36 84 // The timer flips the pin, so the freqency should be twice the target freqence
#define TIMER_CYCLES_38 82 // The timer flips the pin, so the freqency should be twice the target freqence
#define LOOPS_PER_10_US 14

// The connection point of the IR LED or driver.
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_BIT 0b00000110

// For maximum compactness codes are stored as the parsed data.
// This does mean that differnt tranmiting functions have to be used depending on the protocol
// Hoever, only two main protocols exist: NEC and RC 5

void initTimer0(); // Initialize the timer and set the LED's pin to output
ISR (TIMER0_OVF_vect);

// Transmit a pulse of ontime*10 us wait offtime*10 us.
void transmit_element(int ontime, int offtime);

// High level transmit functions
void transmit_nec(uint8_t address, uint8_t command);
void transmit_necext(uint16_t address, uint16_t command);
void transmit_rc5(uint8_t address, uint8_t command); // Also works for RC5x
void transmit_samsung32(uint16_t address, uint16_t command);
