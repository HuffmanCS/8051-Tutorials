; Created by James Huffman on 1/20/22.
;
; timer2int.s
; Assembly source for an AT89C52/AT89S52 MCU (an Intel 8051 based chip)
;
; The 'delay_ms' function has a total delay period equal to ((12MHz / 12) * PAUSE) milliseconds
; The 8051's in-circuit timer counts at 1/12 the speed of the external crystal oscillator
; With a 12MHz crystal oscillator as the external clock, this translates directly 1/1 as 1 micro-second (us)
; Since 2^16 all set equals 65535, this is the maximum number the 16-bit timer can count to
; Since each count equates to 1us, to achieve 1 millisecond the timer must count 1000 times
; Thus 65535 - 1000 = 64535 = 0xfc17 which will be loaded into the timer's registers to accomplish this
;

$MOD52  ; this is an Assembler Control, which turns on the SFR symbols for 8052 MCUs

PAUSE    EQU 100  ; define numeric constant, with symbol 'PAUSE' as literal 250
counter  EQU 0x30 ; define symbol counter, which will be used to represent the address of a byte variable in RAM

; PROGRAM START ;
ORG 0000H          ; origin 0-Hex, or memory location 0x00 starting here
reset:
    AJMP init ; Absolute jump to label init

; TIMER 2 INTERRUPT ;
ORG 002BH          ; this is the memory location of the vector address for the Timer 2 Overflow Interrupt
t2_int:
    MOV     R0, #counter ;load Register-0 with counter constant (which is a memory address in RAM) 
    INC     @R0         ; increment the counter "variable" located in RAM 
    MOV     A, #0xC8    ; load 200 into Accumulator
    SUBB    A, @R0      ; subtract value at counter from 200 
    JZ      rst_cntr    ; jump if Accumulator is zero
    AJMP    t2_int_exit ; absolute jump to exit label
rst_cntr:
    MOV     @R0, #0x00  ; will reset the counter "variable" to 0
t2_int_exit:
    CLR     T2CON.7     ; TF2 bit - clear the Timer 2 Overflow flag
    RETI                ; return from interrupt

; MAIN ROUTINE ;
ORG 0050H          ; begin main program code at memory location 0x0050 beyond all interrupt vectors
init:
    MOV     R0, #counter  ; initialize the counter "variable" to zero
    MOV     @R0, #0x00
    MOV     T2CON, #0x00  ; setup Timer 2 in 16-bit auto-reload mode
    MOV     TL2, #0xAF 	  ; load the low byte of Timer 2 (65535 - 50000) = 15535 ===> 0x3CAF
    MOV     TH2, #0x3C    ; load the high byte of Timer 2
    MOV     RCAP2L, #0xAF ; contains the low byte of the value to reload Timer 2 with upon overflow
    MOV     RCAP2H, #0x3C ; contains the high byte of the value to reload Timer 2 with upon overflow 
    SETB    EA     	  ; enables all interrupts
    SETB    ET2    	  ; enable Timer 2 overflow interrupt
    SETB    TR2    	  ; start Timer 2
    MOV P2, #0x01	  ; start with Port 2 clear except for bit 0
main:
    MOV     R0, #counter
    MOV     A, @R0      ; load accumulator with counter "variable"
    MOV     B, #0x14    ; load Register-B with 20
    DIV     AB          ; divide Accumulator by Register-B
    MOV     A, B        ; move Register-B to Accumulator (after DIV instruction, B contains remainder)
    JZ      modulo_even ; jump if Accumulator is zero, ie (counter % 20 == 0)
    AJMP    main        ; Absolute jump to label main (ie the main loop)
modulo_even:
    MOV     A, P2       ; move Port 2 value into Accumulator
    XRL     A, #0x01    ; X-OR/toggle the first bit 
    MOV     P2, A       ; move Accumulator value into Port 2
    ACALL   delay_ms    ; absolute call to delay_ms label 
    AJMP    main        ; Absolute jump to label main (ie the main loop)

; DELAY MILLISECONDS ;
delay_ms:
    MOV     R0, #PAUSE          ; load the literal PAUSE into Register-0 (determines delay length, value of PAUSE in ms)
timer_set:
    MOV     TMOD, #0x01         ; set the timer mode to 16-bit mode for Timer 0
    MOV     TH0, #0xFC          ; loads the Timer-0-High Register (for high-byte of 64535)
    MOV     TL0, #0x17          ; loads the Timer-0-Low Register (for low-byte of 64535)
    SETB    TR0                 ; starts Timer 0
timer_count:
    JNB     TF0, timer_count    ; jump if bit not set for Timer-Flag-0 bit (cleared once value in TH-TL overflows to 0)
    CLR     TR0                 ; stops Timer 0
    CLR     TF0                 ; clear Timer-Flag-0 bit
    DJNZ    R0, timer_set       ; decrement and jump if not zero (counts down from whatever value is specified by PAUSE)
    RET

END     ; end of program (must always be at the end of C51 source file)
