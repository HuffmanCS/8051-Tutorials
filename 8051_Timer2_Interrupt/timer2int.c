//  Created by James Huffman on 1/20/22.
//
//  timer2int.c
//  C source for an AT89C52/AT89S52 MCU (an Intel 8051 based chip)
//

// included headers, in particular the 8052.h, which allows SDCC to recognize the use of 8051 and 8052 symbols
#include <8052.h>

// static variable to act as a counter, specifically of intervals of 50ms
static char counter = 0;

// function to handle interrupts caused by Timer 2 overflowing
void timer2overflow(void) __interrupt 5{
    // increment counter everytime an overflow occurs in Timer 2
    counter++;
    // to keep intervals consistent, reset counter at 199 (hence counting 200 times)
    if( counter > 199 )
        counter = 0;
    TF2 = 0; // clear Timer 2 overflow flag
}//end_timer2overflow

// delay function that utilizes the 8051's built-in hardware timer to ensure an accurate millisecond delay
void delay_ms(int ms){
    // each loop creates a 1 millisecond delay
    while(ms > 0){
        TMOD = 0x01;    // set the timer mode to 16-bit mode for Timer 0
        TH0 = 0xfc;     // loads the Timer-0-High Register (for high-byte of 64535)
        TL0 = 0x17;     // loads the Timer-0-Low Register (for low-byte of 64535)
        TR0 = 1;        // starts Timer 0
        while(TF0 == 0);// do nothing while Timer-0-Overflow Flag is not set
        TR0 = 0;        // stop Timer 0
        TF0 = 0;        // clear Timer 0 Overflow flag
        ms--;           // decrement milliseconds passed
    }
}//end_delay_ms

// main routine
void main(void){
    // setup Timer 2 in 16-bit auto-reload mode
    T2CON = 0x00;
    // load the low and high bytes of Timer 2 with (65535 - 50000) = 15535 ===> 0x3CAF
    TL2 = 0xaf;
    TH2 = 0x3c;
    // the following registers will hold the value to reload Timer 2 with upon overflow
    RCAP2L = 0xaf;
    RCAP2H = 0x3c;
    // enable interrupts to occur, specifically for Timer 2 overflow to signal an interrupt
    EA = 1;     // enables all interrupts
    ET2 = 1;    // enable Timer 2 overflow interrupt
    TR2 = 1;    // start Timer 2
    // start with Port 2 clear except for first bit
    P2 = 0x01;
    // the main loop
    while(1){
        // check if 'counter' is evenly divisible by 20 (as 20 * 50ms = 1000ms or 1 second)
        if( counter % 20 == 0 ){
            P2 ^= 0x01;    // toggle the state of the first bit of port 2
            delay_ms(100); // delay to ensure Timer 2 has a chance to change the state of counter
        }
    }
}//end_main
