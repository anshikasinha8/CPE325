; ------------------------------------------------------------------------------
; File:         Lab5_SW_Mult.asm
; Function:     Software multiplication subroutine
; Description:  Computes the dot product of two given arrays using the Shift and 
;                   Add multiplication algorithm.
; Input:        Calling the subroutine
; Output:       The result is returned through the stack
; Author:       Anshika Sinha
; Date:         April 13, 2025
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
; ------------------------------------------------------------------------------
        .def    SW_Mult
        .text

SW_Mult:
    push R4             ; Save registers onto stack
    push R5
    push R6
    push R7
    push R8
    push R9
    push R10
    push R11

    mov 22(SP), R4     ; R4 = pointer to array1
    mov 20(SP), R5     ; R5 = pointer to array2
    mov 18(SP), R6     ; R6 = length
    clr R12            ; R12 = result

SW_Loop:
    jz SW_End       ; If length is 0, end loop

    mov @R4+, R9       ; Load array1 element into R7 (multiplicand)
    mov @R5+, R10       ; Load array2 element into R8 (multiplier)
    clr R11            ; Clear product accumulator
    ;mov R7, R9         ; Copy multiplicand to R9
    ;mov R8, R10        ; Copy multiplier to R10
    mov #16, R7        ; Loop counter

SW_ShiftAdd:
    bit #1, R10
    jz  SW_Shift

    add R9, R11        ; If LSB = 1, add R9 (multiplicand)

SW_Shift:
    rla R9             ; Shift multiplicand left
    rra R10            ; Shift multiplier right
    dec R7
    jnz SW_ShiftAdd

    bit #1, R8
    jz SW_Result
    sub R9, R11        ; If multiplier was negative, subtract

SW_Result:
    add R11, R12       ; Accumulate result
    dec R6
    jmp SW_Loop

SW_End:
    pop R11             ; Restore registers
    pop R10
    pop R9
    pop R8
    pop R7
    pop R6
    pop R5
    pop R4
    ret                 ; Return result
    .end
