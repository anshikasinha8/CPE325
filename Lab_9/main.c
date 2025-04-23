/*--------------------------------------------------------------------------
 * File: main.c
 * Description: Prompts the user to input a frequency value and cycle value
 *              It then uses these values to blink LED2 at that frequency
 *              for that many cycles. In addition, each time it toggles LED2
 *              a character is sent over SPI to the slave MSP430 causing
 *              LED2 to toggle on the slave. This results in LED2 blinking
 *              at the same time on the master and slave board.
 *
 * Board: 5529
 * Input: UART serial monitor
 * Output: UART serial input, SPI output to slave MSP430, LED2
 * Author: Charles Marmann, Anshika Sinha
 * Date: March 23, 2025
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char MST_Data,SLV_Data;
char askFreq[] = "You Can Set the Frequency of LED-2 from 1 - 15 Hz by entering the value.\n\rWhat is your intended frequency?\n\r"; //
char askCycle[] = "What is your intended cycle?\n\r"; //
char strIn[10];     // Holds the value of the read string
int frequency = 1;  // Frequency that the LEDs should blink at
int cycle = 0;      // Number of cycles that the LEDs should blink
int cycleCount = 1; // Number of cycles completed
int ledCount = 0;   // Iterator for the WDT ISR

void UART_setup(void) {
    //P3SEL |= BIT3 | BIT4;       // Set UC0TXD and UC0RXD to transmit and receive data
    P4SEL |= BIT4 + BIT5;   // Set USCI_A1 RXD/TXD to receive/transmit data
    UCA1CTL1 |= UCSWRST;    // Set software reset during initialization
    UCA1CTL0 = 0;           // USCI_A1 control register
    UCA1CTL1 |= UCSSEL_2;   // Clock source SMCLK

    UCA1BR0 = 0x09;         // 1048576 Hz  / 115200 lower byte
    UCA1BR1 = 0x00;         // upper byte
    UCA1MCTL = 0x02;        // Modulation (UCBRS0=0x01, UCOS16=0)
    UCA1CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}

void SPI_Master_UCB0_Setup(void) {
    P3SEL |= BIT0+BIT1+BIT2;                       // P3.0,1,2 option select for SIMO,SOMI and CLK
    UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCB0CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 = 18;
    UCB0BR1 = 0;                              //
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void WDT_setup()
{
    WDTCTL = WDT_MDLY_0_5;      // Set WDT to all an interrupt every 0.5 ms
}

void SPI_sendChar(char bip)
{
    while (!(UCB0IFG&UCTXIFG)); // USCI_A0 TX buffer ready?
    UCB0TXBUF = bip; // Transmit first character
}

void UART_sendChar(char bip)
{
    while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA1TXBUF = bip;                  // TX -> RXed character
}

//
//  UART code from lab 8
//

char UART_getChar()
{
    while (!(UCA1IFG&UCRXIFG));
    if (UCA1RXBUF == '\r')
    {
        UART_sendChar('\r');
        UART_sendChar('\n');
    }
    else
    {
        UART_sendChar(UCA1RXBUF);
    }

    return UCA1RXBUF;
}

void UART_sendString(char* str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        UART_sendChar(str[i]);
        i++;
    }
}

void UART_getLine(char* buf, int limit)
{
    int i = 0;
    while (limit != 0)
    {
        char temp = UART_getChar();
        if (temp == '\r')
        {
            break;
        }
        buf[i] = temp;
        i++;
        limit--;
    }
    // Add null character at the end
    buf[i] = '\0';
}

//
// Restrict both frequency and cycle
//

void setFreq(int input)
{
    if (input > 15)
    {
        frequency = 15;
    }
    else if (input < 1)
    {
        frequency = 1;
    }
    else
    {
        frequency = input;
    }
}

void setCycle(int input)
{
    if (input < 0)
    {
        cycle = 0;
    }
    else
    {
        cycle = input;
    }
    cycleCount = 1;
}

int main() {
    WDT_setup();
    UART_setup();                // Initialize USCI_A0 module in UART mode
    SPI_Master_UCB0_Setup();

    // Setup LEDs
    P1OUT = 0;
    P1DIR |= BIT0;
    P4DIR |= BIT7;
    P4OUT &= ~BIT7;
    P1IN &= ~BIT2;

    // Initialize interrupts
    _EINT();

    P1DIR |= BIT0;                // Set P1.0 to output direction

    while (!(P1IN&BIT2)); // wait for the slave to connect

    while(1)
    {
        UART_sendString(askFreq);   // Ask for frequency
        UART_getLine(strIn,10);     // Receive frequency
        setFreq(atoi(strIn));       // Convert to int

        UART_sendString(askCycle);  // Ask for cycle
        UART_getLine(strIn,10);     // Receive cycles
        setCycle(atoi(strIn));      // Convert to int, reset cycleCount

        SFRIE1 |= WDTIE;            // Enable WDT interrupt

    }
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void) {
    ledCount++;                         // Increment ledCount
    int temp = 1000/frequency;          // 2 * 1000 * 0.5 ms = 1s

    if (cycleCount < ((cycle)*2)+1)     // If the number of cycles ran is less than the desired ammount of cycles
    {

        if (ledCount > temp) {
            P4OUT ^= BIT7;              // Toggle master LED
            cycleCount++;               // increment cycleCount
            ledCount = 0;               // Reset ledCount
            SPI_sendChar('A');          // Send a character to toggle the slave LED
            while(!(UCB0IFG&UCRXIFG));  // Wait for a character to come back
        }
    }
    else                                // All cycles completed
    {
        P4OUT &= ~BIT7;                 // Turn off master LED
        SFRIE1 &= ~WDTIE;               // Disable WDT ISR
    }


}
