/*==============================================================================

 =============================================================================*/

#include    "xc.h"              // XC compiler general include file

#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include	"RAINBO.h"			// Include user-created constants and functions
#include    "stdlib.h"          //include stdlib for random function
// Byte variables for NeoPixel functions

unsigned char red = 1; // RGB colour bytes and default starting colour
unsigned char green = 255;
unsigned char blue = 1;
unsigned char startValue = 50;

const char maxLEDs = 8; // Number of LEDs in the string

unsigned char temp; // Temporary byte storage
unsigned char i; // Generic index counter

unsigned char maxBrightness = 200;
unsigned char tempValue;
unsigned char index = 0;
unsigned char redArray[8];
unsigned char blueArray[8];
unsigned char greenArray[8];
unsigned char randArray[8];

unsigned char tempRand;
unsigned char finalRed;
unsigned char finalBlue;
unsigned char finalGreen;

unsigned char receivedData;
bool hasrecieved = false;

enum State {
    still,
    touched
};

void neoArray(unsigned char red[], unsigned char blue[], unsigned char green[]) {
    for (unsigned char index = 0; index < 8; index++) {
        temp = green[index]; // Copy green byte, prepare to shi ft MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = red[index]; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = blue[index]; // Blue last. 
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
    }
}

/*==============================================================================
 *	neoRGB - sets requested number of Neopixel LEDs to same RGB value.
 =============================================================================*/

void neoRGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char leds) {
    for (leds; leds != 0; leds--) // Repeat all 24 colour bits for each LED
    {
        temp = green; // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = red; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = blue; // Blue last. 
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
    }
}

//populates array with random offset values

void popRandArray() {
    for (int i = 0; i < maxLEDs; i++) {
        randArray[i] = rand() % (255);
    }
}

void arrayPulse() {
    for (int i = 0; i < maxLEDs; i++) {
        if (blueArray[i] < randArray[i]) {
            redArray[i]++;
            greenArray[i]++;
            blueArray[i]++;
        }
        if (blueArray[i] > randArray[i]) {
            redArray[i]--;
            greenArray[i]--;
            blueArray[i]--;
        }
        if (blueArray[i] == randArray[i]) {
            randArray[i] = startValue;
        }
        if (randArray[i] == startValue) {
            randArray[i] = rand() % 255;
        }
        __delay_us(75);
    }
}
//function that allows the leds to brighten and darken smoothly

void pulse() {
    tempRand = rand() % (255 - blue); // sets a random value for lights to brighten, makes sure the values doesn't go over 255
    finalRed = red;
    finalGreen = green;
    finalBlue = blue;
    finalRed += tempRand;
    finalBlue += tempRand;
    finalGreen += tempRand;
    while (blue < finalBlue) {
        for (int i = 0; i < maxLEDs; i++) {
            redArray[i]++;
            greenArray[i]++;
            blueArray[i]++;
        }
        neoArray(redArray, greenArray, blueArray);
        __delay_ms(20);
    }
    finalRed -= tempRand;
    finalGreen -= tempRand;
    finalBlue -= tempRand;
    while (blue > finalBlue) {
        for (int i = 0; i < maxLEDs; i++) {
            redArray[i]--;
            blueArray[i]--;
            greenArray[i]--;
        }
        neoArray(redArray, greenArray, blueArray);
        __delay_ms(20);
    }
}
//function that fills the array of RGB on startup

void populateArray() {
    for (int i = 0; i < maxLEDs; i++) {
        redArray[i] = 50;
        blueArray[i] = 50;
        greenArray[i] = 50;
    }
}

void populateArray(unsigned char number) {
    for (unsigned char i = 8; i != 0; i--) {
        //takes the most significant bit from binary number and checks if it's 1
        if ((number & 0b10000000) == 0b10000000) {
            //if 1 sets corresponding bit value to 1 (255) or 0 (0).
            blueArray[i] = 255;
        } else {
            blueArray[i] = 0;
        }
        //shifts the number one bit to the left
        number = number << 1;
    }
}

void recArray(unsigned char data) { //function that lights up 2 leds if the corresponding pad is touched 
    for (int i = 0; i < 8; i += 2) {
        if (data & 0b00000001 == 1) { //checks if lsb is 1
            redArray[i] = 255;
            redArray[i + 1] = 255;
            greenArray[i] = 255;
            greenArray[i + 1] = 255;
            blueArray[i] = 255;
            blueArray[i + 1] = 255;
        }
        data >> 1; //shifts number to check new bit
    }
}

int main(void) // Start of program
{
    init(); // Initialize oscillator and I/O Ports
    initEUSART(); //Initialize EUSART receiver stuff;
    N1 = 0;
    __delay_us(200);
    enum State state;
    state = still;
    populateArray();

    while (1) {
        //8 bit recieve code
        if (RCIF) {
            receivedData = RCREG;
            populateArray(receivedData); //displays bits of received byte on LEDs
            if(receivedData == 0){
                state = still;
            }
            else{
                state = touched;
            }
        }
        
        if(state == still){
            pulse();
        }
        if(state == touched) {
            neoArray(redArray, greenArray, blueArray);
        }
        //testing code with individual bits
        //        if (recieved == 0b00000000) {
        //            neoRGB(255, 255, 255, maxLEDs);
        //            __delay_ms(25);
        //        }
        //        if (recieved == 0b00000001) {
        //            neoRGB(255, 0, 0, maxLEDs);
        //            __delay_ms(25);
        //        }
        //        if (recieved == 0b00000010) {
        //            neoRGB(0, 255, 0, maxLEDs);
        //            __delay_ms(25);
        //        }
        //        if (recieved == 0b00000100) {
        //            neoRGB(0, 0, 255, maxLEDs);
        //            __delay_ms(25);
        //        }
        //        if (recieved == 0b00001000) {
        //            neoRGB(255, 0, 255, maxLEDs);
        //            __delay_ms(25);
        //        }



    }
}
