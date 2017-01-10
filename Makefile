MCU=atmega32
F_CPU=3686400

BINDIR=bin
SRCDIR=src
INCDIR=../avr-dev/drivers/hd44780/include/
LIBS=../avr-dev/drivers/hd44780/bin/hd44780_atmega32_3686400.a

CC=avr-gcc
CFLAGS=-Wall -O2 -g3 -std=c99

OBJCOPY=avr-objcopy
OBJOPT=-O ihex -j .text -j .data

all:
	$(CC) $(CFLAGS) -mmcu=$(MCU) -DF_CPU=$(F_CPU) \
		-I$(INCDIR) $(SRCDIR)/main.c $(LIBS) -o $(BINDIR)/firmware.elf

	$(OBJCOPY) $(OBJOPT) $(BINDIR)/firmware.elf $(BINDIR)/firmware.hex

clean:
	rm -f $(BINDIR)/*.a
	rm -f $(BINDIR)/*.o
	rm -f $(BINDIR)/*.hex
	rm -f $(BINDIR)/*.elf

