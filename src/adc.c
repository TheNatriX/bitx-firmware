#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "mcu.h"

extern void inline
adc_init(void)
{
	/*	Set VREF 2.56V			*/
	ADMUX = (uint8_t) (1 << REFS1) | (1 << REFS0);

	/*	Set Prescaler Fmclk/32		*/
	/*	FIXME: 16mhz cpu => Fmclk/128	*/
	/*	Enable ADC Interrupt		*/
	ADCSRA = (uint8_t) (1 << ADIE) | (1 << ADPS2) | (1 << ADPS0);
}

extern void
adc_start_conversion(uint8_t pin)
{
	/*
	 * Since sleep_* macros will modify the MCUCR register,
	 * we need to restore it back later.
	 * */
	uint8_t register saved = MCUCR;

	/*	Using Single Conversion for pin	*/
	ADMUX &= 0xe0;
	ADMUX |= (uint8_t) (pin & 0x1f);

	/*	Enable ADC Circuitry		*/
	ADCSRA |= (uint8_t) (1 << ADEN);

	/*	ADC Noise Reduction Sleep Mode	*/
	set_sleep_mode(SLEEP_MODE_ADC);
	sleep_enable();
	sei();
	sleep_cpu();

	/*	Disable ADC Circuitry		*/
	ADCSRA &= (uint8_t) ~(1 << ADEN);

	/*	Restore MCUCR			*/
	MCUCR = saved;
}

