

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd.h"
#include "mcu.h"
#include "spi.h"
#include "rf_platform.h"


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
process_event(void)
{

#define LCD_FREQ_POSITION	7

	char buffer[16];
	switch (event) {
		case DIAL_UP:
		frequency.hz += frequency.step;
		dds_write_freq(FREQ_TO_PLATFORM(frequency.hz));
		break;

		case DIAL_DOWN:
		frequency.hz -= frequency.step;
		dds_write_freq(FREQ_TO_PLATFORM(frequency.hz));
		break;

		case PUSH_BTN:
		frequency.step *= 10;
		if (frequency.step == 1000000)
			frequency.step = 1;
		break;
	}
	sprintf(buffer, "%lu", frequency.hz);
	lcd_send_instr(LCD_INSTR_SET_DDRAM | LCD_FREQ_POSITION);
	lcd_print(buffer);
	clear_events();
}


static void inline
frequency_init(void)
{
	frequency.hz = 3705000;
	frequency.step = 1000;
}


static void inline
light_init(void)
{
	// TODO: make it more elegant!
	DDRC |= (1 << PC0);
	PORTC |= (1 << PC0);
}


int main(void)
{
	uint16_t skip_voltage_reading = 0;

	/* TODO: READ EEPROM FOR SAVED VALUES */

	spi_init();
	frequency_init();
	dds_init(FREQ_TO_PLATFORM(frequency.hz));
	adc_init();
	encoder_init();
	lcd_init(LCD_SET_TWO_LINES);
	light_init();


	/* TODO: CONFIG SMETER */

	sei();

	for (;;) {
		/* FIXME: this delay should be implemented by timer,
		 * while putting CPU to sleep.
		 */
		_delay_ms(10);

		/* Display battery voltage */
		skip_voltage_reading++;
		if (skip_voltage_reading == 1000) {
			skip_voltage_reading = 0;
			show_voltage();
		}

		/* TODO: READ Smeter */

		if (event)
			process_event();
	}
	return -1;
}

