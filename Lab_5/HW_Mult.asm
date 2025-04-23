; ------------------------------------------------------------------------------
; File:         Lab5_HW_Mult.asm
; Function:     Hardware Multiplication subroutine
; Description:  Calculates dot product using the hardware multiplier
; Input:        Calling the subroutine
; Output:       The result is returned through the stack
; Author:       Anshika Sinha
; Date:         April 13, 2025
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
; ------------------------------------------------------------------------------
        .def    HW_Mult
        .text

HW_Mult:
    push R4             ; Save registers onto stack
    push R5
    push R6

    mov 12(SP), R4     ; R4 = array1 pointer
    mov 10(SP), R5      ; R5 = array2 pointer
    mov 8(SP), R6      ; R6 = array length
    clr R13            ; Clear result register

HW_Loop:
    mov @R4+, R9      ; Load element from array1 into R9
    mov @R5+, R10      ; Load element from array2 into R10

    mov R9, MPY      ; Load element from array1 into MPY
    mov R10, OP2      ; Load element from array2 into OP2
    
    nop
    nop
    nop
    add RESLO, R13     ; Add lower result to sum
    dec R6
    jnz HW_Loop

DotHW_End:              
    pop R6              ; Restore registers
    pop R5
    pop R4
    ret                 ; Return result
    .end
