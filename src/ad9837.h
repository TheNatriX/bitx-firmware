

/* AD9837 Control Register Bits */
#define	D15	15	/* Register select; Control/FREQ0/FREQ1/PHASE0/PHASE1	*/
#define D14	14	/* Register select; Control/FREQ0/FREQ1/PHASE0/PHASE1	*/
#define B28	13	/* Load frequency registers by two write operations	*/
#define HLB	12	/* MSB(1)/LSB(0) frequency register loading		*/
#define FSEL	11	/* Select FREQ0/FREQ1 register				*/
#define PSEL	10	/* Select PHASE0/PHASE1 register			*/
#define RESET	8	/* This bit controls the reset function			*/
#define SLEEP1	7	/* Enables/Disables the internal MCLK			*/
#define SLEEP12	6	/* Enable/Disable the internal DAC			*/
#define OPBITEN	5	/* Connect/Disconect DAC to VOUT			*/
#define DIV2	3	/* MSB/2 of the DAC data to be output at the VOUT pin	*/
#define MODE	1	/* Use/bypass SIN_ROM; 0 = sinwave / 1 = triangle	*/

