#include <avr/io.h>
#include "mcu.h"
#include "spi.h"


extern void inline
spi_init(void)
{
	uint8_t tmp;

	/* Set SS, MOSI, SCK output */
	DDR_SPI = (1 << SS) | (1 << MOSI) | (1 << SCK);
	/* Set SPI Control Register */
	SPCR = ((1 << SPIE)	| /*	SPI Interrupt Enable	*/
		(1 << SPE)	| /*	SPI Enable		*/
		(0 << DORD)	| /*	Data order 1 = LSB first*/
		(1 << MSTR)	| /*	SPI Master/Slave	*/
		(0 << CPOL)	| /*	Clock Polarity		*/
		(1 << CPHA)	| /*	Clock Phase		*/
		(0 << SPR1)	| /*	SPI Clock Rate select	*/
		(0 << SPR0));	  /*	SPI Clock Rate select	*/

	/* TODO: Do we really need this??
	 * Clear SPI Interrupt Flag */
	tmp = SPSR & SPIF;
	tmp = SPDR;

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


