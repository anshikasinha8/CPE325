//******************************************************************************
//   MSP430F552x Demo - USCI_A0, SPI 3-Wire Slave Data Echo
//
//   Description: SPI slave demo using 3-wire mode. Incrementing
//   data is sent by the master starting at 0x01. Received data is expected to
//   be same as the previous transmission.
//   Initialize SPI Slave mode, as follows:
//   3-wire mode, clock polarity is high, MSB is sent first.
//   Slave generated a logic high pulse on P1.2 indicating it is ready.
//   Communication is handled in the infinite loop, as follows:
//      Once a new character is received it is echoed if TXBUF is ready.
//
//
//
//
//
//           LED2 is toggled providing visual indication of communication.
//                      MSP430F552x
//                  -------------------
//          LED1<-|P1.0                 |
//Slave is Ready<-|P1.2                 |
//                |                     |
//                |                     |
//                |                     |
//                |                     |
//                |                 P3.0|<- Data In (UCA0SIMO)
//                |                     |
//                |                 P3.1|-> Data Out (UCA0SOMI)
//                |                 P3.2|-> Serial Clock Out (UCB0CLK)
//
//
//   Author: A. Milenkovic, milenkovic@computer.org
//
//   Date: October 2022
//******************************************************************************
#include <msp430.h>
/*
void SPI_Slave_UCA0_Setup(void) {
    P3SEL |= BIT3+BIT4; // P3.3,4 option select
    P2SEL |= BIT7; // P2.7 option select
    UCA0CTL1 |= UCSWRST; // **Put state machine in reset**
    UCA0CTL0 |= UCSYNC+UCCKPL+UCMSB; // 3-pin, 8-bit SPI slave,
    // Clock polarity high, MSB
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
}
*/
void SPI_Slave_UCB0_Setup(void) {
    P3SEL |= BIT0+BIT1+BIT2; // P3.3,4 option select
    //P2SEL |= BIT7; // P2.7 option select
    UCB0CTL1 |= UCSWRST; // **Put state machine in reset**
    UCB0CTL0 |= UCSYNC+UCCKPL+UCMSB; // 3-pin, 8-bit SPI slave,
    // Clock polarity high, MSB
    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
}
int main(void) {
    WDTCTL = WDTPW+WDTHOLD;

    SPI_Slave_UCB0_Setup();
    P1DIR |= BIT2 + BIT0; // Set P1.0 and P1.2 as outputs
    P1OUT |= BIT2 + BIT0; // LED1 is on, P1.2 is set
    __delay_cycles(100);
    //P1OUT &= ~BIT2;  // LED is on, P1.2 is off
    // P4.7 is heartbeat of the application (toggles on each received char)
    P4DIR |= BIT7;
    P4OUT = 0;
    for(;;) {
        while(!(UCB0IFG&UCRXIFG));  // wait for a new character
        while(!(UCB0IFG&UCTXIFG));  // new character is received, is TXBUF ready?
        UCB0TXBUF = UCB0RXBUF;      // echo character back if ready
        P4OUT ^= BIT7;
    }
}
