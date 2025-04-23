;-------------------------------------------------------------------------------
; * File:        main.asm
; * Function:    Calculate dot product of two arrays
; * Description: This MSP430 program calculates the dot product of two given arrays. 
; The two input arrays are initialized with 6 values each, ranging between -32 and +32.
;
; * Input:       Two arrays with at least 6 values ranging from -32 to +32
; * Output:      Pass the result on program stack
; * Author(s):   Anshika Sinha
; * Date:        April 13, 2025
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
            .ref    SW_Mult
            .ref    HW_Mult
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET:      mov.w   #__STACK_END,SP         ; Initialize stackpointer
            mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

main:
            ; CALL HW_Mult
            push    #ARR1                   ; Push address of 2nd array onto stack
            push    #ARR2                   ; Push address of 1st array onto stack
            push    #6                      ; Push array length onto stack
            call    #HW_Mult
            add     #6, SP                  ; Clean up stack (3 pushes * 2 bytes)

            ; CALL SW_Mult 
            push    #ARR1                   ; Push address of 2nd array onto stack
            push    #ARR2                   ; Push address of 1st array onto stack
            push    #6                      ; Push array length onto stack
            call    #SW_Mult
            add     #6, SP

            jmp     $                       ; Infinite loop
            nop                             ; Required for debug

.data
ARR1:     .int -1, 1, 1, 1, 1, 0
ARR2:     .int 2, 2, 2, 2, -4, 5

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
