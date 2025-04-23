/*-------------------------------------------------------------------------
 * File:        Lab10_P3.c
 * Function:    Generate waveforms using a Digital to Analog circuit
 * Description: This C program generates waveforms using a Digital-to-Analog 
                circuit (DAC) connected to the MSP430 and alter them using 
                the switches.
 * Input:       DAC circuit connected to the MSP430
 * Output:      When no switches are held: sine waveform using lookup table
                When Switch 1 is held: triangular waveform
                When Switch 2 is held: the frequency doubles
 * Author(s):   Anshika Sinha
 * Date:        03/25/25
 *-----------------------------------------------------------------------*/
#include <msp430.h>
#include "Assignment_p3_sine_lut_256.h"

#define SW1 (P2IN & BIT1)
#define SW2 (P1IN & BIT1)

unsigned char i = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Timer B Configuration
    TB0CCTL0 = CCIE;            // Timer Count Triggers Interrupt
    TB0CTL = TBSSEL_2 + MC_1;   // SMCLK is source, and set to up mode
    TB0CCR0 = 29;              // Cause the interrupt to trigger 30 times per second
    P3DIR = 0xFF;               // Enable all pins on Port 3 as outputs.
    P3OUT = 0x00;

    // Configure Switch 1
    P2DIR &= ~BIT1;             // Set P2.1 as input for SW1 input
    P2REN |= BIT1;              // Enable pull-up register at P2.1
    P2OUT |= BIT1;              // Set register

    // Configure Switch 2
    P1DIR &= ~BIT1;             // Set P1.1 as input for SW1 input
    P1REN |= BIT1;              // Enable pull-up register at P1.1
    P1OUT |= BIT1;              // Set register

    __bis_SR_register(LPM0_bits + GIE);
    return 0;
}

#pragma vector=TIMERB0_VECTOR
__interrupt void timerISR2(void){
    i += 1;              // Increment sine wave index

    if ((SW1) == 0) {       // If Switch1 is pressed
        P3OUT = i;          // Generate triangular waveform
    }
    else if ((SW2) == 0) {  // If Switch2 is pressed
        i += 2;             // Double frequency
        P3OUT = lut256[i];  // Generate sine wave using lookup table
    }
    else {
        P3OUT = lut256[i];  // Generate sine wave using lookup table
                            // when no switches are held
    }
}
