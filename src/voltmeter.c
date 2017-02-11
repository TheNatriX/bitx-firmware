#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "mcu.h"
#include "lcd.h"


#define LCD_VOLTAGE_POSITION		0
#define ADC_TO_BAT_VOLTAGE(x)		(((2.519 * (float) (x)) / 1024) * 7.77)


static uint16_t adc_value;


/* TODO: this ISR must be moved into adc.c */
ISR(ADC_vect)
{
	adc_value = (ADCL & 0xfffc);
	adc_value |= ((ADCH & 0x03) << 8);
}


extern void
show_voltage(void)
{
	char buffer[8];
	static char last_buffer[8] = "";

	/*	Read battery voltage	*/
	adc_start_conversion(PA0); // <--- replace PA0
	/*	Make it human readable	*/
	sprintf(buffer, "%4.1fV", ADC_TO_BAT_VOLTAGE(adc_value));
	/**	Don't write it to LCD if
	 *	it is same with the last one.
	 */
	if (strcmp(buffer, last_buffer)) {
		lcd_send_instr(LCD_INSTR_SET_DDRAM | LCD_VOLTAGE_POSITION);
		lcd_print(buffer);
		strcpy(last_buffer, buffer);
	}
}

