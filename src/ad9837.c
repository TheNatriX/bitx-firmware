#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "ad9837.h"
#include "mcu.h"
#include "spi.h"


uint16_t ControlRegister;


void
dds_write_word(uint16_t word)
{
	SPI_SS_LOW;
	spi_master_send((uint8_t) ((word & 0xff00) >> 8));
	spi_master_send((uint8_t) (word & 0x00ff));
	SPI_SS_HIGH;
}


void
dds_write_freq(uint32_t freq)
{
	uint16_t dds_lsb;
	uint16_t dds_msb;

	FREQ_TO_DDS(freq);

	dds_lsb = GET_14_LSB(freq);
	dds_msb = GET_14_MSB(freq);

	PACKET_HEAD(dds_lsb, FREQ0);
	PACKET_HEAD(dds_msb, FREQ0);

	dds_write_word(dds_lsb);
	dds_write_word(dds_msb);
}


void
dds_clear_phase(void)
{
	uint16_t tmp16 = 0;
	PACKET_HEAD(tmp16, PHASE0);
	dds_write_word(tmp16);
}


void
dds_power_down(void)
{
	ControlRegister |= (uint16_t) (1 << SLEEP1);
	dds_write_word(ControlRegister);
}


void
dds_power_up(void)
{
	ControlRegister &= (uint16_t) ~(1 << SLEEP1);
	dds_write_word(ControlRegister);
}


void
dds_init(uint32_t default_freq)
{
	/* Put the chip in the RESET state */
	ControlRegister = (uint16_t) (1 << B28) | (1 << RESET);
	dds_write_word(ControlRegister);

	/* Set FREQ0 register */
	dds_write_freq(default_freq);

	/* Clear PHASE0 register */
	dds_clear_phase();

	/* Exit RESET */
	ControlRegister &= (uint16_t) ~(1 << RESET);
	dds_write_word(ControlRegister);
}

