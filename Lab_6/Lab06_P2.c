/***************************************************************************
 *   File:        Lab6_P2.c
 *   Description: Initially, LED1 is on and LED2 is off. The clock frequency is set to 2 MHz.
 *                LED1 and LED2 blink using a 50,000 interaction loop delay. Every time SW2 is pressed,
 *                the clock frequency is set to 10 MHz. Every time SW1 is pressed, the clock
 *                frequency is halved, and it cannot go under 1 MHz.
 *   Input:       Press SW1 or SW2
 *   Output:      LED1 and LED2 blink at various different frequencies
 *   Board:       MSP430F5529 Experimenter Board
 *   Author:      Anshika Sinha
 *   Date:        February 18, 2025
***************************************************************************/

#include <msp430.h> 

// Interface inputs and outputs
#define SW1 P2IN&BIT1                           // Switch 1 at P2.1
#define SW2 P1IN&BIT1                           // Switch 2 at P1.1
#define LED1 0x01                               // Mask for BIT0 = 0000_0001b
#define LED2 0x80                               // Mask for BIT7 = 1000_0000b

void setClock(unsigned long freq);
unsigned long curFreq = 2000000;            // Set inital frequency to 2 MHz

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	                // Stop watchdog timer
	// Configure LEDs as outputs
    P1DIR |= LED1;                           // Set P1.0 to output direction
    P4DIR |= LED2;                           // Set P4.7 to output direction

    // Configure switches as inputs
    P2DIR &= ~BIT1;                             // Set P2.1 as input for SW1 input
    P2REN |= BIT1;                              // Enable pull-up register at P2.1
    P2OUT |= BIT1;

    P1DIR &= ~BIT1;                             // Set P1.1 as input for SW2 input
    P1REN |= BIT1;                              // Enable pull-up register at P1.1
    P1OUT |= BIT1;

    P1OUT |= LED1;                             // LED1 is ON
    P4OUT &= ~LED2;                              // LED2 is OFF

    setClock(2000000);                          // Set clock frequency to 2 Hz

    while(1) {
        if ((SW2) == 0)
        {
            setClock(10000000);             // Set clock frequency to 10 MHz
            curFreq = 10000000;             // Update current frequency
            while ((SW2) == 0);             // Wait for release
        }
        if ((SW1) == 0)                         // If SW1 is pressed
        {
            if (curFreq > 1000000) {
                curFreq /= 2;
                setClock(curFreq);
            }
            while ((SW1) == 0);             // Wait for release
        }

        // Toggle LEDs
        P1OUT ^= LED1;
        P4OUT ^= LED2;

        unsigned int i = 0;
        for (i = 50000; i>0; i--);           // 50,000 delay loop
    }
}

void setClock(unsigned long freq) {
    switch (freq) {
            case 10000000:
                UCSCTL1 = DCORSEL_5;
                UCSCTL2 = FLLD_1 + 300;
                break;
            case 5000000:
                UCSCTL1 = DCORSEL_4;
                UCSCTL2 = FLLD_1 + 150;
                break;
            case 2500000:
                UCSCTL1 = DCORSEL_3;
                UCSCTL2 = FLLD_1 + 75;
                break;
            case 2000000:
                UCSCTL1 = DCORSEL_3;
                UCSCTL2 = FLLD_1 + 60;
                break;
            case 1250000:
                UCSCTL1 = DCORSEL_2;
                UCSCTL2 = FLLD_1 + 37;
                break;
            case 1000000:
                UCSCTL1 = DCORSEL_2;
                UCSCTL2 = FLLD_1 + 30;
                break;
    }
}
