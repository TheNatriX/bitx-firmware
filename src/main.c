

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd.h"

struct frequency
{
	uint32_t hz;
	uint32_t step;
} frequency;

static uint8_t event = 0;
#define	DIAL_UP		1
#define DIAL_DOWN	2
#define PUSH_BTN	3

static uint8_t isr_lock = 0;
ISR(INT0_vect)
{
	if (!isr_lock)
		isr_lock = 1;
	else if (!event)
		event = DIAL_DOWN;
}

ISR(INT1_vect)
{
	if (!isr_lock)
		isr_lock = 1;
	else if (!event) 
		event = DIAL_UP;
}

ISR(INT2_vect)
{
	if (!event) event = PUSH_BTN;
}

static void inline clear_events(void)
{
	event = 0;
	isr_lock = 0;
}

static void inline setup_encoder(void)
{
	/* Set INT0, INT1, INT2 as pulled-up input */
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD3);
	DDRB &= ~(1 << PB2);
	PORTD |= (1 << PD2);
	PORTD |= (1 << PD3);
	PORTB |= (1 << PB2);
	/* Set INT0, INT1, INT2 as falling edge interrupt */
	MCUCR |= (1 << ISC01) | (1 << ISC11);
	MCUCR &= ~(1 << ISC00) | (1 << ISC10);
	MCUCSR &= ~(1 << ISC2);
	/* Enable interrupts at INT0, INT1, INT2 */
	GICR |= (1 << INT0 | 1 << INT1 | 1 << INT2);
}

static void inline setup_spi(void)
{
	uint8_t tmp;

	/* Set SS, MOSI, SCK output */
	DDRB = (1 << PB4) | (1 << PB5) | (1 << PB7);
	/* Set SPI Control Register */
	SPCR = ((1 << SPIE)	| /*	SPI Interrupt Enable	*/
		(1 << SPE)	| /*	SPI Enable		*/
		(0 << DORD)	| /*	Data order 1 = LSB first*/
		(1 << MSTR)	| /*	SPI Master/Slave	*/
		(0 << CPOL)	| /*	Clock Polarity		*/
		(1 << CPHA)	| /*	Clock Phase		*/
		(0 << SPR1)	| /*	SPI Clock Rate select	*/
		(0 << SPR0));	  /*	SPI Clock Rate select	*/
	/* Clear SPI Interrupt Flag */
	tmp = SPSR & SPIF;
	tmp = SPDR;
} 

int main(void)
{
	char buffer[100];

	lcd_init(LCD_SET_TWO_LINES);
	lcd_print("Version 0.1.2017");
	lcd_send_instr(LCD_INSTR_SET_DDRAM | 0x40);
	lcd_print("73's  DE  YO3HXT");
	_delay_ms(1000);

	/* TODO: READ EEPROM FOR SAVED VALUES */
	setup_spi();

	/* TODO: SET FREQ */
	/* TODO: CONFIG VOLTMETER */
	/* TODO: CONFIG SMETER */

	setup_encoder();
	sei();

	for (;;) {
		/* FIXME: this delay should be implemented by timer,
		 * while putting CPU to sleep.
		 */
		_delay_ms(1);

		if (event) {
			cli();
			switch (event) {
				case DIAL_UP:
					frequency.hz += frequency.step;
					break;

				case DIAL_DOWN:
					frequency.hz -= frequency.step;
					break;

				case PUSH_BTN:
					/* TODO: step setup */
					break;
			}
			lcd_send_instr(LCD_INSTR_CLEAR_DISPLAY);
			sprintf(buffer, "FREQ: %lu Hz", frequency.hz);
			lcd_print(buffer);
			clear_events();
			sei();
		}
		/* TODO: READ VOLTAGE */
		/* TODO: READ Smeter */
	}
	return -1;
}

