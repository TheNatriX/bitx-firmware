MCU=atmega32
F_CPU=16000000
VERSION=\"v1.21\"

BINDIR=bin
SRCDIR=src
INCDIR=../avr-dev/drivers/hd44780/include/
LIBS=../avr-dev/drivers/hd44780/bin/hd44780_atmega32_3686400.a

CC=avr-gcc
CFLAGS=-Wall -g3 -O2 -pipe -gstabs -std=c99 -Wl,-u,vfprintf -lprintf_flt -lm

OBJCOPY=avr-objcopy
OBJOPT=-O ihex -j .text -j .data

all:
	$(CC) $(CFLAGS) -mmcu=$(MCU)			\
		-DF_CPU=$(F_CPU) -DVERSION=$(VERSION)	\
		-I$(INCDIR)				\
		-o $(BINDIR)/firmware.elf		\
		$(SRCDIR)/main.c			\
		$(SRCDIR)/encoder.c			\
		$(SRCDIR)/tick.c			\
		$(SRCDIR)/voltmeter.c			\
		$(SRCDIR)/spi.c				\
		$(SRCDIR)/adc.c				\
		$(SRCDIR)/ad9837.c			\
		$(LIBS)

	$(OBJCOPY) $(OBJOPT) $(BINDIR)/firmware.elf $(BINDIR)/firmware.hex

clean:
	rm -f $(BINDIR)/*.a
	rm -f $(BINDIR)/*.o
	rm -f $(BINDIR)/*.hex
	rm -f $(BINDIR)/*.elf

