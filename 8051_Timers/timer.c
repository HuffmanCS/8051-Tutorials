//  Created by James Huffman on 1/20/22.
//
//  timer.c
//  C source for an AT89C52/AT89S52 MCU (an Intel 8051 based chip)
//
//  The 'delay_ms' function has a total delay period equal to ((12MHz / 12) * PAUSE) milliseconds
//  The 8051's in-circuit timer counts at 1/12 the speed of the external crystal oscillator
//  With a 12MHz crystal oscillator as the external clock, this translates directly 1/1 as 1 micro-second (us)
//  Since 2^16 all set equals 65535, this is the maximum number the 16-bit timer can count to
//  Since each count equates to 1us, to achieve 1 millisecond the timer must count 1000 times
//  Thus 65535 - 1000 = 64535 = 0xfc17 which will be loaded into the timer's registers to accomplish this
//

// included headers, in particular the 8052.h, which allows SDCC to recognize the use of 8051 symbols
#include <8052.h>

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
    // the main loop
    while(1){
        P2 = 0x01;      // turn on red LED, with green and blue off
        delay_ms(250);  // delay
        P2 = 0x02;      // red off, green on, blue off
        delay_ms(250);  // delay
        P2 = 0x04;      // red off, green off, blue on
        delay_ms(250);  // delay
    }
}//end_main
