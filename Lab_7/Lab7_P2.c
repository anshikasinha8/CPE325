/*--------------------------------------------------------------------------
* File:        Lab7_P2.c
* Function:    Toggle buzzer using Timer_B (MPS430F5529)
*
* Description: This C program for the MSP430F5529 microcontroller generates a 2 kHz sound
*              using Timer B and a connected buzzer, toggling the sound on and off every second.
*
* Clocks:      ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = default (2^20 Hz)
*              An external watch crystal between XIN & XOUT is required for ACLK
* Input:       None
* Output:      Produce buzzer sound at 2 MHz and toggle every 1 second
* Author:      Anshika Sinha
*-------------------------------------------------------------------------*/
#include <msp430F5529.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <intrinsics.h>

volatile int buzzer = 0;

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;    // Stop WDT
    SFRIE1 |= WDTIE;             // Enable Watchdog timer Interrupt

    P7DIR |= BIT4;              // P7.4 output
    P7SEL |= BIT4;              // P7.4 special function (TB0.2 output)
    
    
    TBCCR0 |= 500;              // Value to count upto for Up mode
    TB0CTL = TBSSEL_2 + MC_1;   // ACLK is clock source, UP mode

    WDTCTL = WDT_ADLY_1000;      // 2s interval
    
    _BIS_SR(LPM0_bits + GIE);   // Enter Low Power Mode 3
}

// Watchdog timer ISR
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void) {
    if (buzzer == 1) {
        TB0CCTL2 = OUTMOD_0;              // Start timer in up mode
        buzzer = 0;
    } else {
        TB0CCTL2 = OUTMOD_4;            // Stop Timer B
        buzzer = 1;
    }
}
