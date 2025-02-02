/* -------------------------------------------------------------------------
* File: Lab01_P1.c
* Function: Controlling LED1 and LED2 using switches
* Description: This C program starts with LED1 off and LED2 on. If SW1 is
*               held, LED1 blinks at 11 hz and LED2 turns off. If SW2 is
*               held, LED1 turns on and LED2 blinks at 6 hz.
* Input: Hold Switch 1 or Switch 2
* Output: LED1 blinks at 11 hz and LED2 turns off when Switch1 is held
* and LED1 turns on and LED2 blinks at 6 hz when Switch2 is pressed
*
* Author(s): Anshika Sinha
* Date: 01/22/2025
*--------------------------------------------------------------------------- */
// #include <msp430.h>

// Interface inputs and outputs
#define SW1 P2IN&BIT1                               // Switch 1 at P2.1
#define SW2 P1IN&BIT1                               // Switch 2 at P1.1
#define LED1 0x01                                   // Mask for BIT0 = 0000_0001b
#define LED2 0x80                                   // Mask for BIT7 = 1000_0000b

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                       // Stop watchdog timer

    // Original state with LED1 OFF and LED2 ON
    P1DIR |= LED1;                                  // Set P1.0 to output direction
    P4DIR |= LED2;                                  // Set P4.7 to output direction
    P1OUT &= ~LED1;                                 // Set LED1 to OFF
    P4OUT |= LED2;                                  // Set LED2 to ON

    P2DIR &= ~BIT1;                                 // Set P2.1 as input for SW1 input
    P2REN |= BIT1;                                  // Enable pull-up register at P2.1
    P2OUT |= BIT1;

    P1DIR &= ~BIT1;                                 // Set P1.1 as input for SW2 input
    P1REN |= BIT1;                                  // Enable pull-up register at P1.1
    P1OUT |= BIT1;

    unsigned int i = 0;
    while(1)
    {
        if ((SW1) == 0)                             // If SW1 is pressed
        {
            for (i = 2000; i>0; i--);               // Debounce ~20 ms
            while ((SW1) == 0) {                    // If SW1 is actually held
                for (i = 0; i < 9000; i++);         // 90 ms delay
                P1OUT ^= LED1;                      // Toggle LED1
                P4OUT &= ~LED2;                     // Turn LED2 OFF
            }
        }
        else if ((SW2) == 0)
        {
            for (i = 2000; i>0; i--);               // Debounce ~20 ms
            while ((SW2) == 0) {
                for (i = 0; i < 16600; i++);        // 166 ms delay
                P4OUT ^= LED2;                      // Toggle LED2
                P1OUT |= LED1;                      // Turn LED1 on
            }
        }
        else
        {
            P1OUT &= ~LED1;                         // Turn LED1 off
            P4OUT |= LED2;                          // Turn LED2 on
        }
    }


}