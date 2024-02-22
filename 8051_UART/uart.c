//  Created by James Huffman on 11/14/22.
//
//  uart.c
//  8051 UART functionality
//
// this code is targeted at an AT89C52/AT89S52 MCU to enable the built in UART, though it can work on other 8051 varients
//

// included headers
#include <8052.h>       // allows SDCC to recognize the use of 8051/52 symbols
#include <stdint.h>     // allows usage of standard integer types

uint8_t uart_rx_buffer[32] = {0x00};    // rx buffer for incoming data from host
uint8_t uart_rx_cntr = 0;               // rx counter for indexing next available byte to place incoming data from host into

// function for UART RX, interrupt driven whenever SBUF acquires a byte
void uart_rx(void) __interrupt 4{
    uart_rx_buffer[uart_rx_cntr] = SBUF;    // acquire the received byte in SBUF
    if( uart_rx_cntr < 32 )                 // if buffer is full, incoming data may become lost until serviced
        uart_rx_cntr++;                     // only increment rx counter if below 32
    RI = 0; // clear receive interrupt flag
}//end_uart_rx

// function for UART TX
void uart_tx(uint8_t val){
    EA = 0; // disable interrupts
    SBUF = val;         // place the char to transmit into the SBUF register
    while( TI == 0 );   // wait until transmission is finished
    TI = 0;             // clear transmission interrupt flag
    EA = 1; // enable interrupts
}//end_uart_tx

// delay function that utilizes the 8051's built-in hardware timer to ensure an accurate millisecond delay
void delay_ms(uint32_t ms){
    // each loop creates a 1 millisecond delay
    while(ms > 0){
        //TMOD |= 0x01;   // set the timer mode to 16-bit mode for Timer 0
        TH0 = 0xfc;     // loads the Timer-0-High Register (for high-byte of 64535)
        TL0 = 0x17;     // loads the Timer-0-Low Register (for low-byte of 64535)
        TR0 = 1;        // starts Timer 0
        while(TF0 == 0);// do nothing while Timer-0-Overflow Flag is not set
        TR0 = 0;        // stop Timer 0
        TF0 = 0;        // clear Timer 0 Overflow flag
        ms--;           // decrement milliseconds passed
    }
}//end_delay_ms

// print function for sending string messages over UART (32 bytes max, but can be modified)
void print(char *str){
    // loop until end of string as indicated by null character (or max transmission length reached, ie, 32 bytes)
    int i = 0;
    while( str[i] != '\0' && i < 32 ){
        uart_tx(str[i++]);
    } uart_tx('\r'); uart_tx('\n'); // carriage return and newline for each string transmitted
}//end_print

// main routine
void main(void){
    // setup the ports for I/O operations
    P2 = 0x07; // LED pins
    P3 = 0xff; // pins on port 3 dedicated to UART must be set (specifically 3.0 for RX, 3.1 for TX)
    // initialize UART functionality
    //PCON |= 0x80;   // would double baud rate (so long as Timer 1 is used)
    SCON = 0x50;    // mode 1 is selected (8-bit UART), reception enabled (REN set)
    IE = 0x90;      // enable UART interrupt
    TMOD = 0x21;    // timer 1 in 8-bit auto reload mode, timer 0 in 16-bit mode (used for delay_ms function)
    TH1 = 0xf3;     // loading value 243 for 2400 baud rate
    TR1 = 1;        // start timer 1
    // message to print
    char string[32] = {"Hello there... General Kenobi."};
    print("Start:");
    // main loop
    while(1){
        // check if data was received over UART
        while( uart_rx_cntr > 0 ){
            // decrement counter
            uart_rx_cntr--;
            // check the character at the indexed position in buffer for latest command
            if( uart_rx_buffer[uart_rx_cntr] == '1' ){
                P2_0 ^= 1;
            }else if( uart_rx_buffer[uart_rx_cntr] == '2' ){
                P2_1 ^= 1;
            }else if( uart_rx_buffer[uart_rx_cntr] == '3' ){
                P2_2 ^= 1;
            }else if( uart_rx_buffer[uart_rx_cntr] == '4' ){
                print(string);
            }else{
                delay_ms(1); // delay not vital, just an example
            }
            uart_rx_buffer[uart_rx_cntr] = 0; // clearing byte not vital, just an example
        }//end_while(uart_rx_cntr > 0)
    }//end_while(1)
}//end_main
