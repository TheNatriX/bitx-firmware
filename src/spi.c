#include <avr/io.h>
#include "mcu.h"
#include "spi.h"


extern void inline
spi_init(void)
{
	/* Set SS, MOSI, SCK output */
	DDR_SPI = (1 << SS) | (1 << MOSI) | (1 << SCK);
	/* Set SPI Control Register */
	SPCR = ((0 << SPIE)	| /*	SPI Interrupt Enable	*/
		(1 << SPE)	| /*	SPI Enable		*/
		(0 << DORD)	| /*	Data order 1 = LSB first*/
		(1 << MSTR)	| /*	SPI Master/Slave	*/
		(1 << CPOL)	| /*	Clock Polarity		*/
		(0 << CPHA)	| /*	Clock Phase		*/
		(0 << SPR1)	| /*	SPI Clock Rate select	*/
		(0 << SPR0));	  /*	SPI Clock Rate select	*/

	SPI_SS_HIGH;
} 


extern void
spi_master_send(uint8_t byte)
{
	/* Start transmission */
	SPDR = byte;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)));
}

