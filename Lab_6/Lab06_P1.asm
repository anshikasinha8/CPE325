;---------------------------------------------------------------------------
;	File:       	Lab6_P1.asm
;
;	Description:	An interrupt service routine is used to interface the switches.
;					Initially, both LED1 and LED2 start in the OFF state. When SW1 is
;					first pressed, LED2 turns on and changes state with each subsequent
;					press. When SW2 is pressed, LED1 blinks 5 times at 5 Hz, then changes
;					the state of LED2.
;
;   Author:			Anshika Sinha
;   Date:			February 18, 2025
;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;---------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

            .def    SW_ISR
            .def 	delay
;---------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;---------------------------------------------------------------------------
RESET:       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT:     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;---------------------------------------------------------------------------
; Main loop here
;---------------------------------------------------------------------------
Setup:
			bis.b	#001h, &P1DIR			; Set P1.0 to output direction
			bis.b	#080h, &P4DIR			; Set P4.7 to output direction
			bic.b	#001h, &P1OUT			; Set P1OUT to 0x0000_0001 (off)
			bic.b	#080h, &P4OUT			; Set P4OUT to 0x1000_0000 (off)
            
			bic.b	#002h, &P2DIR			; Set P2.1 as input for SW1
			bis.b	#002h, &P2REN			; Enable pull-up resistor at P2.1
			bis.b	#002h, &P2OUT			; Required for I/O setup

			bic.b	#002h, &P1DIR			; Set P1.1 as input for SW2
			bis.b	#002h, &P1REN			; Enable pull-up resistor at P1.1
			bis.b	#002h, &P1OUT			; Required for I/O setup

			bis.w	#GIE, SR				; Enable global interrupts

			bis.b	#002h, &P1IE			; Enable Port1 interrupt
			bis.b	#002h, &P1IES			; Set interrupt call from hi to low
			bic.b	#002h, &P1IFG			; Clear interrupt flag

			bis.b	#002h, &P2IE			; Enable Port2 interrupt
			bis.b	#002h, &P2IES			; Set interrupt call from hi to low
			bic.b	#002h, &P2IFG			; Clear interrupt flag

InfLoop:
			nop
			jmp		$						; Loop until interrupt

;----------------------------------------------------------------------------
; P1_0 (SW2) and P4_7 (SW1) interrupt service routine (ISR)
;----------------------------------------------------------------------------
SW_ISR:
            bic.b	#002, &P2IFG			; Clear Interrupt flag
            bit.b   #002h, &P2IN           ; Check SW2
            jz		CheckSW1				; If not, check SW1
            bic.b	#002h, &P1IFG			; Clear interrupt flag

CheckSW2:   bit.b   #02h, &P1IN             ; Check if SW2 is pressed
                                            ; (0000_0010 on P1IN)
            jnz     lExit                   ; If not zero, SW is not pressed
                                            ; loop and check again
Debounce2:   mov.w   #2000, R15              ; Set to (2000 * 10 cc )
SW2D20ms:    dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW2D20ms                 ; Delay over?
            bit.b   #00000010b,&P1IN        ; Verify SW2 is still pressed
            jnz     lExit                   ; If not, wait for SW2 press
LED1on:      mov.w	#9, R4					; Blink 5 times
blinkLoop:
            xor.b   #0x01,&P1OUT            ; Toggle LED1

            call	#delay
            dec.w	R4
            jnz		blinkLoop
            xor.b	#0x80, &P4OUT			; Toggle LED2

SW2wait:    bit.b   #002h,&P1IN             ; Test SW2
            jz      SW2wait                 ; Wait until SW2 is released
            bic.b   #001,&P1OUT             ; Turn off LED1
            jmp 	lExit

CheckSW1:	bit.b	#002h, &P2IN			; Check if SW1 is pressed
            jnz		lExit


Debounce1:  mov.w   #2000, R15              ; Set to (2000 * 10 cc )
SW1D20ms:   dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW1D20ms                 ; Delay over?
            bit.b   #002h,&P2IN        		; Verify SW2 is still pressed
            jnz     lExit                   ; If not, wait for SW2 press

LED2on:     xor.b	#0x80, &P4OUT			; Toggle LED2
SW1wait:    bit.b   #002h,&P1IN             ; Test SW1
            jz      SW1wait                 ; Wait until SW1 is released
            jmp 	lExit

lExit:		reti							; return from interrupt

;-------------------------------------------------------------------------------
; Delay subroutine
;-------------------------------------------------------------------------------
delay:		mov		#50000, R5
			jmp 	delayLoop
delayLoop:
			dec 	R5
			jnz		delayLoop
			ret

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            .sect   ".int47"                ;PORT1_VECTOR
            .short	SW_ISR
            .sect	".int42"	      ;PORT2_VECTOR
            .short  SW_ISR
            .end
