; Created by James Huffman on 1/20/22.
;
; timer.s
; Assembly source for an AT89C52/AT89S52 MCU (an Intel 8051 based chip)
;
; The 'delay_ms' function has a total delay period equal to ((12MHz / 12) * PAUSE) milliseconds
; The 8051's in-circuit timer counts at 1/12 the speed of the external crystal oscillator
; With a 12MHz crystal oscillator as the external clock, this translates directly 1/1 as 1 micro-second (us)
; Since 2^16 all set equals 65535, this is the maximum number the 16-bit timer can count to
; Since each count equates to 1us, to achieve 1 millisecond the timer must count 1000 times
; Thus 65535 - 1000 = 64535 = 0xfc17 which will be loaded into the timer's registers to accomplish this

; PROGRAM START ;
ORG 0H          ; origin 0-Hex, or memory location 0x00 starting here

PAUSE EQU 250   ; define numeric constant, with symbol 'PAUSE' as literal 250

main:
    ACALL   red_on      ; Absolute call at label 'red_on'
    ACALL   grn_off 
    ACALL   blu_off 
    ACALL   delay_ms    ; delay
    ACALL   red_off
    ACALL   grn_on 
    ACALL   delay_ms    ; delay 
    ACALL   grn_off 
    ACALL   blu_on 
    ACALL   delay_ms    ; delay 
    AJMP    main        ; Absolute jump to label main (ie the main routine)

red_on:
    SETB    P2.0    ; set bit 0 on Port 2
    RET             ; return from sub-routine
red_off:
    CLR     P2.0    ; clear bit 0 on Port 2
    RET             
grn_on:
    SETB    P2.1    ; set bit 1 on Port 2
    RET              
grn_off:
    CLR     P2.1    ; clear bit 1 on Port 2
    RET             
blu_on:
    SETB    P2.2    ; set bit 2 on Port 2
    RET             
blu_off:
    CLR     P2.2    ; clear bit 2 on Port 2
    RET             

delay_ms:
    MOV     R0, #PAUSE          ; load the literal PAUSE into Register-0 (determines delay length, value of PAUSE in ms)
timer_set:
    MOV     TMOD, #0x01         ; set the timer mode to 16-bit mode for Timer 0
    MOV     TH0,  #0xfc         ; loads the Timer-0-High Register (for high-byte of 64535)
    MOV     TL0,  #0x17         ; loads the Timer-0-Low Register (for low-byte of 64535)
    SETB    TR0                 ; starts Timer 0
timer_count:
    JNB     TF0, timer_count    ; jump if bit not set for Timer-Flag-0 bit (cleared once value in TH-TL overflows to 0)
    CLR     TR0                 ; stops Timer 0
    CLR     TF0                 ; clear Timer-Flag-0 bit
    DJNZ    R0, timer_set       ; decrement and jump if not zero (counts down from whatever value is specified by PAUSE)
    RET

END     ; end of program (must always be at the end of C51 source file)
