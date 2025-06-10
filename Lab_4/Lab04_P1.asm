;-------------------------------------------------------------------------------
; File:        Lab04_P1.asm
; Function:    Count digits and characters in string
; Description: ...
; Input:       The input string defined in myStr
; Output:      Length of given string and Total number of digits
; Author(s):   Anshika Sinha
; Date:        01/29/2025
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

myStr: .string "Welcome 2 the MSP430 Assembly, Spring 2025!", ''
									; '' ensures NULL character follows string
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET:      mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
main:   bis.b   #0FFh, &P1DIR         	; Output the the length of string on port pin P1
		bis.b	#0FFh, &P2DIR			; Output the number of digits on port pin P2
        mov.w   #myStr, R4              ; Load the starting address of the string into R4
        clr.b   R5                      ; Register R5 will serve as a character counter
        clr.b	R7						; Register R7 will serve as a digit counter

count:  mov.b   @R4+, R6                ; Load next character
        cmp.b   #0, R6                  ; Check for null character
        jeq     write                   ; If yes, go to the end
        inc.w   R5                      ; If not, increment counter

        cmp.b	#'0', R6				; Check if character is >= '0'
        jlo		count					; Jump if lower
        cmp.b	#'9'+1, R6				; Check if character is <= '9'+1
        jhs		count					; Jump if higher
        inc.w	R7						; If digit, increment counter

        jmp     count                   ; Go to the next character

write:  mov.b   R5,&P1OUT               ; Write character count in P1OUT
		mov.b	R7,&P2OUT				; Write digit count in P2OUT
        bis.w   #LPM4, SR               ; LPM4
        nop                             ; Required only for debugger
                                            

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
            
