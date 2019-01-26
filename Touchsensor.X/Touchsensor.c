/*==============================================================================
	Project: RAINBO-Touch
	Version: 1.01				Date: November 19, 2018
	Target: RAINBO-TX			Processor: PIC12F1840

 RAINBO-TX board capacitive touch sensing program.
 =============================================================================*/

#include    "xc.h"              // XC compiler general include file

#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include	"RAINBO-TX.h"		// Include hardware constants and functions

// Capacitive Sensing Module (CPS) variables for touch tensors T1-T4

unsigned int Ttemp;				// Temporary variable to initialize touch averages
unsigned char Tcount[4];		// CPS oscillator cycle counts for each touch sensor
unsigned char Tavg[4];			// Average counts for each touch sensor
unsigned char Ttrip[4];			// Trip points for each touch sensor
unsigned char Tdelta[4];		// Difference of touch from average sensor count
unsigned char Tactive;			// Number of active touch targets
unsigned char newTarget;		// Most likely new touch target (centroid of touch?)
unsigned char Target;			// Past touch target
signed char Tdir;				// Direction and magnitude of finger motion
const char up = 1;				// Motion direction constants
const char hold = 0;
const char down = -1;

/*==============================================================================
 *	Tpulse - Touch pulse to visualize touch sensor activation.
 *============================================================================*/

void Tpulse(void)
{
   // Tcount[0] = 255;
    for(unsigned char i = 0; i != 4; i++)
    {
        LED1 = 1;
//        __delay_ms(1);
        for(unsigned char c = Tcount[i]; c != 0; c--)
        {
            _delay(2);
        }
        LED1 = 0;
        __delay_ms(1);
    }
}
void bitDelay(){
    __delay_us(56);
}
bool isTouched(unsigned char tcount){
    return (tcount < 70);
}
//function for rs232
void transmitBits(void){
    //using 9600 baud rate the frequency is 104.16666 us
    //found out that when touched tCount value hovers around 40
    //with room for error a 1 is triggered if tcount is less than 70
    LED1 = 1;
    bitDelay();
    for(unsigned char i = 0; i<4; i++){
        if(isTouched(Tcount[i])){
            LED1 = 1;
            bitDelay();
        }
        else {
            LED1 = 0;
            bitDelay();
        }
    }
    for(unsigned char i = 0; i!=4; i++){
        LED1 = 0;
        bitDelay();
    }
    LED1 = 0;
    bitDelay();
    
}
/*==============================================================================
 *	initTouch - Calibrate touch sensor average (non-pressed) values.
 *============================================================================*/

void initTouch(void)
{
	for(unsigned char i = 0; i != 4; i++)
	{
		CPSCON1 = i;				// Sense each of the 4 touch sensors in turn
		Ttemp = 0;					// Reset temporary counter
		for(unsigned char c = 16; c != 0; c--)
		{
			TMR0 = 0;				// Clear capacitive oscillator timer
			__delay_ms(1);			// Wait for fixed sensing time-base
			Ttemp += TMR0;			// Add capacitor oscillator count to temp
		}
		Tavg[i] = Ttemp / 16;		// Save average of 16 cycles
	}
}

/*==============================================================================
 *	Main program loop. The main() function is called first by the C compiler.
 *============================================================================*/

int main(void)
{
	init();						// Initialize oscillator, I/O, and peripherals
	initTouch();				// Calibrate capacitive touch sensor averages
	LED1= 0;
	// Repeat the main loop continuously
	
	while(1)
	{
		SWDTEN = 0;				// Disable Watch Dog Timer
		
		Tactive = 0;			// Reset touch counter
		
		for(unsigned char i = 0; i != 4; i++)	// Check touch pads for new touch
		{
			CPSCON1 = i;			// Select each of the 4 touch sensors in turn
			TMR0 = 0;				// Clear cap oscillator cycle timer
			__delay_ms(2);			// Wait for fixed sensing time-base
			Tcount[i] = TMR0;		// Save current oscillator cycle count
			Ttrip[i] = Tavg[i] / 4;	// Create 75% trip point from previous average
			if(Tcount[i] < (Tavg[i] - Ttrip[i]))
			{
				Tactive ++;			// Increment active count for tripped sensors
				newTarget = i + 1;	// Save current touch target by real number
				Tdelta[i] = Tavg[i] - Tcount[i];	// Calculate touch delta
			}
			else
			{
				Tdelta[i] = Tavg[i] - Tcount[i];	// Calculate delta, update average
				Tavg[i] = Tavg[i] - (Tavg[i] / 16) + (Tcount[i] / 16);
			}
		}
		
		if(Tactive == 0)			// Nothing pressed? Reset target.
		{
			newTarget = 0;
		}
		
		// If Tactive == 1, we already know the target button.
		
		if(Tactive > 1)				// Select most likely target if 2+ are active
		{
			if(Tdelta[1] > Tdelta[2])	// Compare centre two targets first, then
			{							// 1 & 2 or 3 & 4
				newTarget = 2;			// Adjust for real target number  (0-3 -> T1-T4)
				if(Tdelta[0] > Tdelta[1])
					newTarget = 1;
			}
			else
			{
				newTarget = 3;
				if(Tdelta[3] > Tdelta[2])
					newTarget = 4;
			}
		}
		        
		Target = newTarget;
		
        //Tpulse();
        transmitBits();			
		SWDTEN = 1;				// Enable Watch Dog Timer
		//SLEEP();				// Nap to save power. Wake up in ~32 ms.
	}
}
