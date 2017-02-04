#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "mcu.h"

extern void inline
adc_init(void)
{
	/*	Set VREF 2.56V			*/
	ADMUX = (uint8_t) (1 << REFS1) | (1 << REFS0);

	ADCSRA = (uint8_t) (1 << ADIE) |	/* Enable ADC Interrupt */
			(1 << ADEN)    |	/* Enable ADC Circuitry */
			(1 << ADPS2)   |	/* ADC Clock Prescaler  */
			(1 << ADPS1)   |	/* ADC Clock Prescaler  */
			(1 << ADPS0);		/* ADC Clock Prescaler  */
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

	/*	ADC Noise Reduction Sleep Mode	*/
	set_sleep_mode(SLEEP_MODE_ADC);
	sleep_enable();
	sei();
	sleep_cpu();

	/*	Restore MCUCR			*/
	MCUCR = saved;
}

