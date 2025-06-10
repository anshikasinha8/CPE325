;-------------------------------------------------------------------------------
; File:        Lab04_P2.asm
; Function:    Multiplication by addition
; Description: ...
; Input:       Input string of format "x*y" defined in myStr
; Output:      Integer product of two numbers
; Author(s):   Anshika Sinha
; Date:        02/02/2025
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
myStr:		.string "4*7", ''
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
main:	bis.b 	#0FFh, &P1DIR			; Set P1 as output
		mov.w	#myStr, R4				; Load starting address of string into R4

		clr.b	R10						; Clear result
		; Check validity of string
		mov.b	@R4+, R5				; Load first character into R5
		cmp.b 	#'0', R5				; Check if char is >= '0'
		jlo		error					; Jump to error if lower
		cmp.b	#'9'+1, R5				; Check if  char is <= '9'+1
		jhs		error					; Jump to error if higher or same

		mov.b	@R4+, R6				; Load next character
		cmp.b	#'*', R6				; Check if '*'
		jne 	error					; If not, jump to error

		mov.b	@R4+, R7				; Load second operand
		cmp.b 	#'0', R7				; Check if char is >= '0'
		jlo		error					; Jump to error if lower
		cmp.b	#'9'+1, R7				; Check if  char is <= '9'+1
		jge		error					; Jump to error if higher or same

		mov.b	@R4+, R6				; Load next character in string
		cmp.b	#0, R6					; Check if null character
		jne 	error					; If not, jump to error

		; Convert ASCII to int
		sub.b	#'0', R5				; Convert first operand to int
		sub.b	#'0', R7				; Convert second operand to int

		; Find smaller int
		cmp.b	R7, R5					; Check R5 > R7
		jlo		swap					; swap if first is smaller
		mov.b	R7, R8					; Smaller number in R8
		mov.b	R5, R9					; Larger number in R9
		jmp 	multiply

swap:
		mov.b	R5, R8
		mov.b 	R7, R9

multiply:
		cmp		#0, R8
		jeq 	done					; Exit if counter < 0
		add.b	R9, R10					; Add larger number to result
		dec 	R8						; Decrease loop counter
		jmp 	multiply

done:   mov.b   R10,&P1OUT               ; Write result in P1OUT
lend:   bis.w   #LPM4, SR               ; LPM4
        nop                             ; Required only for debugger

error:	mov.b	#0FFh, &P1OUT			; Output 0xFF if input is invalid
		jmp 	lend
		nop

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
            .end
            
