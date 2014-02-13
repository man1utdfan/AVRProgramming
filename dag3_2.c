/*
 * Project name:
     Demo3_2 : Counter with preset on Counter/Timer 2
 * Author: Avans-TI, JW
 * Revision History: 
     20101222: - initial release;
 * Description:
     This program counts events on pin T2 = PD7.
	 the result is in register TCNT2, shown on output port B
 * Test configuration:
     MCU:             ATmega128
     Dev.Board:       BIGAVR6
     Oscillator:      External Clock 08.0000 MHz
     Ext. Modules:    -
     SW:              AVR-GCC
 * NOTES:
     - Turn ON the PORT LEDs at SW12.1 - SW12.8
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BIT(x)	(1 << (x))
#define TimerStart -10

unsigned int tenthValue = 0;

// wait(): busy waiting for 'ms' millisecond
// Used library: util/delay.h
void wait( int ms )
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms( 1 );			// library function (max 30 ms at 8MHz)
	}
}

// Interrupt routine timer2 overflow
//
ISR( TIMER2_OVF_vect )
{
	TCNT2 = TimerStart;			// Preset value
	tenthValue++;				// Increment counter
}

// Initialize timer 2: counting, preset, interrupt on overflow
void timer2Init( void )
{
	TCNT2 = TimerStart;			// Preset value of counter 2
	TIMSK |= BIT(6);			// T2 overflow interrupt enable
	SREG |= BIT(7);				// turn_on intr all
	TCCR2 = 0x07;				// Initialize T2: ext.counting, rising edge
}

// Main program: Counting on T2
int main( void )
{
	DDRD &= ~BIT(7);			// set PORTD.7 for input
	DDRB = 0xFF;				// set PORTB for output (shows countregister)
	DDRC = 0xFF;				// set PORTC for output (shows tenthvalue)
	timer2Init();

	while (1)
	{
		PORTB = TCNT2;			// show value counter 2
		PORTC = tenthValue;		// show value tenth counter
		wait(10);				// every 10 ms
	}
}

	