
#include <avr/io.h>
#include "mcu.h"

extern void inline
encoder_init(void)
{
	/* Set INT0, INT1, INT2 as pulled-up input */
	DDR_INT0 &= ~(1 << PIN_INT0);
	DDR_INT1 &= ~(1 << PIN_INT1);
	DDR_INT2 &= ~(1 << PIN_INT2);
	PORT_INT0 |= (1 << PIN_INT0);
	PORT_INT1 |= (1 << PIN_INT1);
	PORT_INT2 |= (1 << PIN_INT2);
	/* Set INT0, INT1, INT2 as falling edge interrupt */
	MCUCR |= (1 << ISC01) | (1 << ISC11);
	MCUCR &= ~(1 << ISC00) | (1 << ISC10);
	MCUCSR &= ~(1 << ISC2);
	/* Enable interrupts at INT0, INT1, INT2 */
	GICR |= (1 << INT0 | 1 << INT1 | 1 << INT2);
}



