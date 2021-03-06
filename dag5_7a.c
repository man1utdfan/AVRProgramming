/* spi Serial Peripheral Interface
 *	 7 segments display
 *  Master
 *	WvdE
 * 24 febr 2011
 */
/*
 * Project name		: Demo5_5 : spi - 7 segments display
 * Author			: Avans-TI, WvdE, JW
 * Revision History	: 20110228: - initial release;
 * Description		: This program sends data to 1 4-digit display with spi
 * Test configuration:
     MCU:             ATmega128
     Dev.Board:       BIGAVR6
     Oscillator:      External Clock 08.0000 MHz
     Ext. Modules:    Serial 7-seg display
     SW:              AVR-GCC
 * NOTES			: Turn ON switch 15, PB1/PB2/PB3 to MISO/MOSI/SCK
*/

#include <avr/io.h>
#include <util/delay.h>

#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB					// spi Data direction register
#define PORT_SPI	PORTB					// spi Output register
#define SPI_SCK		1						// PB1: spi Pin System Clock
#define SPI_MOSI	2						// PB2: spi Pin MOSI
#define SPI_MISO	3						// PB3: spi Pin MISO
#define SPI_SS		0						// PB0: spi Pin Slave Select

// wait(): busy waiting for 'ms' millisecond
// used library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
		{
			_delay_ms(1);
		}
}

void spi_masterInit(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
											// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
											// Mode = 0: CPOL=0, CPPH=0;
}


// Write a byte from master to slave
void spi_write( unsigned char data )				
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
char spi_writeRead( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
	data = SPDR;							// New received data (eventually, MISO) in SPDR
	return data;							// Return received byte
}

// Select device on pinnumer PORTB
void spi_slaveSelect(unsigned char chipNumber)
{
	PORTB &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slaveDeSelect(unsigned char chipNumber)
{

	PORTB |= BIT(chipNumber);
}


// Initialize the driver chip (type MAX 7219)
void displayDriverInit() 
{
	spi_slaveSelect(0);				// Select display chip (MAX7219)
  	spi_write(0x09);      			// Register 09: Decode Mode
  	spi_write(0xFF);				// 	-> 1's = BCD mode for all digits
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select dispaly chip
  	spi_write(0x0A);      			// Register 0A: Intensity
  	spi_write(0x04);    			//  -> Level 4 (in range [1..F])
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0B);  				// Register 0B: Scan-limit
  	spi_write(0x03);   				// 	-> 1 = Display digits 0..1
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display on ('normal operation')
void displayOn() 
{
  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display off ('shut down')
void displayOff() 
{
  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x00); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

void spi_writeWord ( unsigned char adress, unsigned char data )
{
		spi_slaveSelect(0);         // Select display chip
		spi_write(adress);         		// 	digit adress: (digit place)
		spi_write(data);  			// 	digit value:  (digit place)
		spi_slaveDeSelect(0); 		// Deselect display chip
}

void writeLedDisplay( int value )
{
	if (value>0)
	{
			value = value%10000;
			if (value>999)
			{
				spi_writeWord(4,value/1000);
			}
			if (value>99)
			{
				spi_writeWord(3,value%1000/100);
			}
			if (value>9)
			{
				spi_writeWord(2,value%100/10);
			}

			spi_writeWord(1,value%10);
	}
	else
	{
			value = value * -1;
			value = value%1000;

			writeDash(4);

			if (value>99)
			{
				spi_writeWord(3,value%1000/100);
			}
			if (value>9)
			{
				spi_writeWord(2,value%100/10);
			}

			spi_writeWord(1,value%10);
	}

}

void writeDash(int dashLoc)
{
		spi_slaveSelect(0);         			// Select display chip
		spi_write(dashLoc%5);         			// 	digit adress: (digit place)
		SPDR = 0b1010;							// Load byte to Data register --> starts transmission
		while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete
		spi_slaveDeSelect(0); 					// Deselect display chip
}

void writeDecimalLedDisplay(double value)
{


	if (value>0)
	{
			int highValue = value;
			int decimalValue = (value-highValue)*100;

			highValue = highValue%100;
			if (highValue>9)
			{
				spi_writeWord(4,highValue%100/10);
			}

			spi_writeWord(3,highValue%10+0x80);
			
			if (decimalValue>9)
			{
				spi_writeWord(2,decimalValue%100/10);
			}
			spi_writeWord(1,decimalValue%10);

	}
	else
	{
			value = value * -1;


			int highValue = value;
			int decimalValue = (value-highValue)*100;

			writeDash(4);
			highValue = highValue%100;

			spi_writeWord(3,highValue%10+0x80);
			
			if (decimalValue>9)
			{
				spi_writeWord(2,decimalValue%100/10);
			}
			spi_writeWord(1,decimalValue%10);
	}
}

int main()
{
	// inilialize
	DDRB=0x01;					  	// Set PB0 pin as output for display select
	spi_masterInit();              	// Initialize spi module
	displayDriverInit();            // Initialize display chip

 	// clear display (all zero's)
	for (char i =1; i<=4; i++)
	{
		spi_writeWord(i,0);
	}    
	wait(1000);

	writeDecimalLedDisplay(-12.3456);

	wait(1000);



  	return (1);
}
