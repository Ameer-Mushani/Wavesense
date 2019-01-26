/*==============================================================================
	RAINBO-TX (PIC12F1840) symbolic constants for main and other user functions.
==============================================================================*/

// I/O Port devices

#define S1			RA3			// Switch S1 input
#define LED			RA5			// LED1 output
#define LED1		RA5			// LED1 output

//Capacitive touch sensor input channel constants

unsigned char T1 = 0;           // CPS channel constants for each touch sensor
unsigned char T2 = 1;           // Used to select channels in CPSCON1 register
unsigned char T3 = 2;
unsigned char T4 = 3;

// Clock frequency for delay macros and simulation

#define _XTAL_FREQ	4000000     // Set clock frequency for time delay calculations
#define FCY	_XTAL_FREQ/4        // Processor instruction cycle time

// TODO - Add function prototypes for all functions in RAINBO-TX.c here:

void init(void);                // Initialization function prototype