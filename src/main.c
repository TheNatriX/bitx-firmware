

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd.h"
#include "mcu.h"
#include "spi.h"
#include "rf_platform.h"

#define ADC_TO_BAT_VOLTAGE(x)	(((2.519 * (float) (x)) / 1024) * 7.77)

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
static uint16_t bat_voltage = 0;
static uint16_t last_bat_voltage = 0;


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


ISR(ADC_vect)
{
	bat_voltage = ADCL;
	bat_voltage |= ((ADCH & 0x03) << 8);
	bat_voltage &= 0xfffc;
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


static void inline
process_event(void)
{
	char buffer[100];

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
	lcd_send_instr(LCD_INSTR_CLEAR_DISPLAY);
	sprintf(buffer, "%4.1fV  %lu",
			ADC_TO_BAT_VOLTAGE(bat_voltage),
			frequency.hz);
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
	char buffer[100];
	/* TODO: READ EEPROM FOR SAVED VALUES */

	spi_init();
	frequency_init();
	dds_init(FREQ_TO_PLATFORM(frequency.hz));
	adc_init();
	encoder_init();
	lcd_init(LCD_SET_TWO_LINES);
	light_init();


	/* TODO: CONFIG VOLTMETER */
	/* TODO: CONFIG SMETER */

	sei();

	for (;;) {
		/* FIXME: this delay should be implemented by timer,
		 * while putting CPU to sleep.
		 */
		_delay_ms(10);

		/* TODO: READ VOLTAGE */
		adc_start_conversion(PA0);
		if (bat_voltage != last_bat_voltage) {
			lcd_send_instr(LCD_INSTR_RETURN_HOME);
			sprintf(buffer, "%4.1fV  %lu",
					ADC_TO_BAT_VOLTAGE(bat_voltage),
					frequency.hz);
			lcd_print(buffer);
			last_bat_voltage = bat_voltage;
		}

		/* TODO: READ Smeter */

		if (event)
			process_event();
	}
	return -1;
}

