/*==============================================================================
	RAINBO-TX (PIC12F1840) hardware initialization function.
==============================================================================*/

#include	"xc.h"				// XC compiler general include file

#include	"stdint.h"			// Integer definition
#include	"stdbool.h"			// Boolean (true/false) definition

#include	"RAINBO-TX.h"		// Include hardware constant definitions

void init(void)
{
	// Initialize oscillator
	
	OSCCON = 0b01101000;		// PLL off, 4 MHz HF internal oscillator
	
	// Initialize user ports and peripherals:

	OPTION_REG = 0b00101000;	// Weak pull-ups on, falling INT interrupt,
								// TRM0 internal from CPS, 1:1 (no pre-scaler)
	WPUA = 0b00001000;			// Enable weak pull-up on RA3 (S1 input)
	PORTA = 0;					// Clear port input registers and output latches
	LATA = 0;					// before configuring port pins
	ANSELA = 0b00010111;		// Set AN0-4 as analogue inputs for touch sensors
	TRISA = 0b00011111;			// Set RA5 as digital output for LED
	
	CPSCON0 = 0b10001001;//0b10001001		// Enable Cap Sense module, fixed reference, TMR0
	CPSCON1 = 0;				// Select capacitive channel 0 (T1)

	WDTCON = 0b00001010;		// WDT off, div 1024 (~32ms period)

	// TODO Enable interrupts if 
    
	
}

