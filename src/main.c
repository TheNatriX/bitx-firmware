

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd.h"
#include "mcu.h"

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

static void inline
clear_events(void)
{
	event = 0;
	isr_lock = 0;
}

static void inline
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

static void process_event(void)
{
	char buffer[100];
	switch (event) {
		case DIAL_UP:
		frequency.hz += frequency.step;
		dds_write_freq(frequency.hz);
		break;

		case DIAL_DOWN:
		frequency.hz -= frequency.step;
		dds_write_freq(frequency.hz);
		break;

		case PUSH_BTN:
		frequency.step *= 10;
		if (frequency.step == 1000000L)
			frequency.step = 1;
		break;
	}
	lcd_send_instr(LCD_INSTR_CLEAR_DISPLAY);
	sprintf(buffer, "FREQ: %lu Hz", frequency.hz);
	lcd_print(buffer);
	clear_events();
}


static void inline
frequency_init(void)
{
	frequency.hz = 3705000;
	frequency.step = 10;
}

int main(void)
{

	lcd_init(LCD_SET_TWO_LINES);
	lcd_print("Version 0.1.2017");
	lcd_send_instr(LCD_INSTR_SET_DDRAM | 0x40);
	lcd_print("73's  DE  YO3HXT");
	_delay_ms(1000);

	/* TODO: READ EEPROM FOR SAVED VALUES */

	spi_init();
	frequency_init();
	dds_init(frequency.hz);

	adc_init();

	/* TODO: CONFIG VOLTMETER */
	/* TODO: CONFIG SMETER */

	encoder_init();
	sei();

	for (;;) {
		/* FIXME: this delay should be implemented by timer,
		 * while putting CPU to sleep.
		 */
		_delay_ms(1);

		cli();
		if (event)
			process_event();
		else {
			/* TODO: READ VOLTAGE */
			/* TODO: READ Smeter */
		}
		sei();
	}
	return -1;
}

