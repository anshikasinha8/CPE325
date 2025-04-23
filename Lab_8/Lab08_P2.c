/*--------------------------------------------------------------------------------
 * File:          Lab08_P2.c
 * Function:      Generate triangular wave using serial
 * Description:   This program generates a triangular wave and displays it on the UAH
 *                Serial App using a 57,600 baud rate. The amplitude of the wave is 8 units
 *                and the frequency is 2.5 Hz. The signal is transmitted for 4 signal
 *                periods.
 *
 * Input:         None
 * Output:        Triangular wave on UAH Serial App
 *
 * Instructions:  Set the following parameters in putty/hyperterminal
 * Port: COM1
 * Baud rate: 57600
 * Data bits: 8
 * Parity: None
 * Stop bits: 1
 * Flow Control: None
 *
 * Author:      Anshika Sinha
 * Date:        March 4, 2025
--------------------------------------------------------------------------------*/
#include <msp430.h> 
#include <stdint.h>
#include <stdbool.h>

// Global variables
volatile float myData = 0.0;            // Stores the current value of the waveform
int i = 0;                              // Counter to track the number of completed periods (full wave cycles)
bool peak = true;                       // Flag to indicate if the waveform is rising (true) or falling (false)

// Function to set up UART communication
void UART_setup(void) {
    P3SEL |= BIT3 | BIT4;               // Configure P3.3 (TXD) and P3.4 (RXD) for UART functionality
    UCA0CTL1 |= UCSWRST;                // Put USCI_A0 in reset mode to configure settings
    UCA0CTL1 |= UCSSEL_2;               // Use SMCLK (1 MHz) as the clock source for UART
    UCA0BR0 = 18;                       // Set the lower byte of the baud rate divider (1 MHz / 57600)
    UCA0BR1 = 0;                        // Set the upper byte of the baud rate divider to 0
    UCA0MCTL = UCBRF_0 | UCBRS_2; // Configure modulation for accurate baud rate timing
    UCA0CTL1 &= ~UCSWRST; // Release USCI_A0 from reset to begin operation
}
// Function to transmit a character over UART
void sendChar(char c) {
    while (!(UCA0IFG & UCTXIFG));       // Wait until the UART transmit buffer is ready
    UCA0TXBUF = c;                      // Load the character into the transmit buffer to send it
}
// Main function to set up the system and enter low power mode
int main() {
    WDTCTL = WDT_MDLY_32;               // Configure the Watchdog Timer for 32 ms interval interrupts
    UART_setup();                       // Initialize UART for communication
    SFRIE1 |= WDTIE;                    // Enable interrupts for the Watchdog Timer
    myData = 0.0;                       // Initialize waveform value to 0 (start of rising phase)
    __bis_SR_register(LPM0_bits + GIE); // Enter Low Power Mode 0 with global interrupts enabled
}
// Watchdog Timer ISR - Handles the generation and transmission of the triangular wave
#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void) {
    char *dataPtr = (char*)&myData;     // Pointer to access the bytes of the floating-point waveform value
    sendChar(0x55); // Send a start byte (optional) to indicate the beginning of the packet

    // Loop to send each byte of the floating-point value over UART
    int j;
    for (j = 0; j < 4; j++) {
        sendChar(dataPtr[j]);           // Send one byte of the floating-point value
    }

    // Update the waveform value based on the current phase (rising or falling)
    if (peak) {
        myData += 0.04;                 // Increase the value by 0.04 units in the rising phase
        if (myData >= 8.0) {            // Check if the peak amplitude is reached
            peak = false;               // Switch to the falling phase
        }
    } else {
        myData -= 0.04;                 // Decrease the value by 0.04 units in the falling phase
        if (myData <= 0.0) {            // Check if the waveform has reached the minimum value
            peak = true;                // Switch back to the rising phase
            i++;                        // Increment the period counter after a complete up/down cycle
        }
    }
    // Stop the waveform generation after completing 4 full periods
    if (i == 4) {
        SFRIE1 &= ~WDTIE;               // Disable Watchdog Timer interrupts to stop waveform generation
    }
}
