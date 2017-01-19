#include <avr/io.h>
#include <avr/sleep.h>
#include "mcu.h"

extern void inline
adc_init(void)
{
	/* Clear ADMUX and Set VREF 2.56V */
	ADMUX = (1 << REFS1) | (1 << REFS0);

	/* Clear ADCSRA.
	 * Set Prescaler Fmclk/32 FIXME: for 16mhz cpu, prescaler = Fmclk/128.
	 * Enable ADC Interrupt.
	 * Turn ON ADC Circuitry.
	 */
	ADCSRA = (1 << ADIE) | (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);
}

extern void
adc_start_conversion(uint8_t pin)
{
	/* Using Single Conversion Mode */
	ADMUX |= (pin & 0x1f);

	/* Enable MCU Sleep and ADC Noise Reduction Sleep Mode */
//	MCUCR |= (1 << SE) | (1 << SM0);

	/* Enter Sleep Mode */
//	sleep_cpu();
}

