//******************************************************************************
//   MSP430F5529 Demo Program - USCI_A0, SPI 3-Wire Master Incremented Data
//   Description: SPI master talks to SPI slave using 3-wire mode. Incrementing
//   data is sent by the master starting at 0x01. Received data is expected to
//   be same as the previous transmission.
//   Once UCA0 is initialized in SPI Master mode, as follows:
//   BRCLK=SMCLK/2, 3-wire mode, clock polarity is high, MSB is sent first.
//   The main loop is entered if P1.2 is at logic 1 which indicates that
//   the slave device is ready.
//   Communication is handled in the infinite loop, as follows:
//      A new character in MST_Data is written into TXBUF if it is empty.
//      if the received data corresponds to previously sent character,
//      the communication is carried out properly, LED1 is on, LED2 toggles.
//      Otherwise, LED1 is off, LED2 is off.
//      The MST_Data and SLV_Data are updated, delay is applied so we
//      can verify program behavior through LED1&LED2.
//
//                   MSP430F552x
//                 -----------------
//                |                 |
//                |                 |
//                |             P1.0|-> LED1
//                |                 |
//                |             P3.0|-> Data Out (UCA0SIMO)
//                |                 |
//                |             P3.1|<- Data In (UCA0SOMI)
//                |                 |
//  Slave RDY   ->|P1.2         P3.2|-> Serial Clock Out (UCB0CLK)
//
//
//   A. Milenkovic, milenkovic@computer.org
//
//   October 2024
//******************************************************************************
#include <msp430.h>
unsigned char MST_Data,SLV_Data;
unsigned char temp;
/*
void SPI_Master_UCA0_Setup(void) {
    P3SEL |= BIT3+BIT4;                       // P3.3,4 option select
    P2SEL |= BIT7;                            // P2.7 option select
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    //UCA0BR0 = 0x02;                           // /2
    UCA0BR1 = 0;                              //
    UCA0MCTL = 0;                             // No modulation
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}
*/
void SPI_Master_UCB0_Setup(void) {
    P3SEL |= BIT0+BIT1+BIT2;                       // P3.0,1,2 option select for SIMO,SOMI and CLK
    UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCB0CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 = 18;
    UCB0BR1 = 0;                              //
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}
int main(void) {
    WDTCTL = WDTPW+WDTHOLD;
    P1OUT = 0;
    P1DIR |= BIT0;
    P4DIR |= BIT7;
    P4OUT &= ~BIT7;
    P1IN &= ~BIT2;
    SPI_Master_UCB0_Setup();
    // Wait for Slave
    while (!(P1IN&BIT2)); // Wait until Slave is ready
    P1OUT |= BIT0; // If ready, light LED1
    __delay_cycles(1000000);
    P1OUT &= ~BIT0;
    MST_Data = 0x01; // Initialize data values
    SLV_Data = 0x00;
    for (;;) {
        while (!(UCB0IFG&UCTXIFG)); // USCI_A0 TX buffer ready?
        UCB0TXBUF = MST_Data; // Transmit first character
        while(!(UCB0IFG&UCRXIFG)); // Wait for data back
        if (UCB0RXBUF==SLV_Data){  // Test for correct character RX'd
            P1OUT |= BIT0; // If correct, light LED1
        }
        else {
            P1OUT &= ~BIT0; // If incorrect, turn off LED1
        }
        P4OUT ^= BIT7; // heart bit on LED2
        MST_Data = (MST_Data + 1) % 50;
        SLV_Data = (SLV_Data + 1) % 50;
        __delay_cycles(1000000);
    }
}
