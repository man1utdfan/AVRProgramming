/* Project name:     Demo2_4 : demo LCD display
 * Author: TI-Avans, WvdE
 * Revision History: 
     20101220: - initial release;
 * Description:
     This is a demo of writing text to the LCD2x16 display 
	 the diaplay is connected to PORTD (see: manual BIGAVR6)
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

#define BIT(x)	(1 << (x))

char regel1[]= "dit is een demo ";
char regel2[]= "van LCD-display ";

// wait(): busy waiting for 'ms' millisecond
// Used library: util/delay.h
void wait( int ms )
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}

// lcdCommand: schrijf command byte (RS=0)
// eerst hoge nibble, dan de lage
// 
void lcdCommand ( unsigned char command )
{
	PORTC = command & 0xF0;		// hoge nibble
	PORTC = PORTC | 0x08;		// start (EN=1)
	wait (1);					// wait 1 ms
	PORTC = 0x00;				// stop
	
	PORTC = (command & 0x0F) << 4;	// lage nibble, shift lage naar hoge bits
	PORTC = PORTC | 0x08;		// start (EN =1)
	wait (1);					// wait 1 ms
	PORTC = 0x00;				// stop (EN=0 RS=0)	
}

// Schrijf data byte (RS=1)
// eerst hoge nibble, dan de lage
// 
void lcdWriteChar( unsigned char dat )
{
	PORTC = dat & 0xF0;			// hoge nibble
	PORTC = PORTC | 0x0C;		// data (RS=1), start (EN =1)
	wait (1);					// wait 1 ms
	PORTC = 0x04;				// stop
		
	PORTC = (dat & 0x0F) << 4;		// lage nibble
	PORTC = PORTC | 0x0C;		// data (RS=1), start (EN =1)
	wait (1);					// pulse 1 ms
	PORTC = 0x00;				// stop (EN=0 RS=0)
}


// Initialisatie LCD
//
void init_lcd(void)
{
	lcdCommand( 0x02 );			// return home
	lcdCommand( 0x28 );			// mode: 4 bits interface data,
							// 2 lines,5x8 dots
	lcdCommand( 0x0C );			// display: on, cursor off, blinking off
	lcdCommand( 0x06 );			// entry mode: cursor to right, no shift
	lcdCommand( 0x80 );			// RAM adress: 0, first position, line 1
}

//
// write upper line
void lcd_writeLine1 ( char text1[] )
{
	// eerst de eerste 8 karakters = regel 1
	lcdCommand(0x80);			// first position, line 1, adress $00
	for ( int i=0; i<16; i++ )
	{
		lcdWriteChar( text1[i] );
	}
} 

//
// write second line
void lcd_writeLine2 ( char text2[] )
{
	// dan de tweede 8 karakters = regel 2
	lcdCommand(0xC0);			// first position, line 2, adress $40

	for ( int i=0; i<16; i++ )
	{
		lcdWriteChar( text2[i] );
	}
} 

//
// shift characters left or right
void lcdShift(int displacement)
{
	// size of the displacement
	int number = displacement<0 ? -displacement : displacement;
	for (int i=0; i<number; i++)
	{
		if (displacement <0)
			lcdCommand(0x18);
		else
			lcdCommand(0x1C);
	}
}

// main program
int main (void)
{
	DDRC=0xFF;					// PORT C is output
	init_lcd();				// initialize LCD

	lcdWriteLine1(regel1);
	lcdWriteLine2(regel2);

	wait(1000);
	lcdShift(-3);
	wait(1000);
	lcdShift(8);

	return 1;
}// end program 