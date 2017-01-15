#include "mcu.h"


#define	SPI_SS_LOW	PORT_SPI &= ~(1 << SS)
#define	SPI_SS_HIGH	PORT_SPI |= (1 << SS)


