/*--------------------------------------------------------------------------------
 * File:          Lab08_P1.c
 * Function:      Medbot Interactive chatbot
 * Description:   Medbot is an interactive chatbot that helps the user prescribe
 *                medication. It prompts the user for their username, continuously
 *                asks what they need medication for, and calculates the total price
 *                for all their medicine.
 *
 * Input:         User input
 * Output:        Medbot chat
 *
 * Instructions:  Set the following parameters in putty/hyperterminal
 * Port: COM1
 * Baud rate: 115200
 * Data bits: 8
 * Parity: None
 * Stop bits: 1
 * Flow Control: None
 *
 * Author:      Anshika Sinha
 * Date:        March 4, 2025
--------------------------------------------------------------------------------*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define colors
#define colorReset   "\x1b[0m"
#define colorBot     "\x1b[31m"  // MedBot in red
#define colorUser    "\x1b[35m"  // User in purple

// Function prototypes
void UART_SETUP();
void UART_sendChar(char bip);
char UART_getChar();
void UART_sendString(char* str);
void UART_getLine(char* buf, int limit);

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
    UART_SETUP();                   // Setup UART

    // Initialize variables
    char userName[50];
    char item[10];
    char response[10];
    char msg[100];
    int totalPrice = 0;

    while(1) {
        // Prompt UserName
        UART_sendString(colorBot "[MedBot]: " colorReset);
        UART_sendString("\r\nHi I am MedBot. What is the name of the patient?\r\n");

        UART_sendString(colorUser "[User]: " colorReset);
        UART_getLine(userName, 50);             // Get username

        // Greet user
        snprintf(msg, sizeof(msg),
                 "What is %s dealing with today? I can prescribe for headache, cough, and allergy.\r\n",
                 userName);
        UART_sendString(colorBot "[MedBot]: " colorReset);
        UART_sendString(msg);

        totalPrice = 0;                         // Reset total price for new prescription

        do {
            // User selects an item
            UART_sendString(colorUser "[User]: " colorReset);
            UART_getLine(item, 10);

            while (strcmp(item, "headache") != 0 && strcmp(item, "cough") != 0 && strcmp(item, "allergy") != 0) {
                snprintf(msg, sizeof(msg),
                         "I cannot prescribe for %s. I can prescribe for headache, cough, and allergy.\r\n",
                         item);
                UART_sendString(colorBot "[MedBot]: " colorReset);
                UART_sendString(msg);

                UART_sendString(colorUser "[User]: " colorReset);
                UART_getLine(item, 10);
            }

            // Handle item price and quantity
            char* med = (strcmp(item, "headache") == 0) ? "aspirin" :
                    (strcmp(item, "cough") == 0) ? "cough drops" : "antihistamines";
            int PRICE = (strcmp(item, "headache") == 0) ? 1 :
                    (strcmp(item, "cough") == 0) ? 2 : 3;

            snprintf(msg, sizeof(msg),
                     "%s costs $%d per dose. How many doses would you like?\r\n", med, PRICE);
            UART_sendString(colorBot "[MedBot]: " colorReset);
            UART_sendString(msg);

            UART_sendString(colorUser "[User]: " colorReset);
            UART_getLine(response, 10);  // Get quantity as input
            int QUANTITY = atoi(response);  // Convert to integer

            totalPrice += PRICE * QUANTITY;  // Update total price

            // Ask if the User needs more medicine
            UART_sendString(colorBot "[MedBot]: " colorReset);
            UART_sendString("Do you need any more medicine?");

            UART_getLine(response, 10);  // Get yes/no response

            // Validate yes/no response
            while (strcmp(response, "yes") != 0 && strcmp(response, "no") != 0) {
                UART_sendString(colorBot "[MedBot]: " colorReset);
                UART_sendString("Please respond with 'yes' or 'no'.\r\n");

                UART_sendString(colorUser "[User]: " colorReset);
                UART_getLine(response, 10);
            }

            // If "yes", re-prompt for the next item
            if (strcmp(response, "yes") == 0) {
                UART_sendString(colorBot "[MedBot]: " colorReset);
                UART_sendString("What is %s dealing with today? I can prescribe for headache, cough, and allergy.\r\n");
            }

        } while (strcmp(response, "yes") == 0);

        // Show total price
        snprintf(msg, sizeof(msg),
                 "The total for all items is $%d. Thank you for prescribing medicine for %s!\r\n\n",
                 totalPrice, userName);
        UART_sendString(colorBot "[MedBot]: " colorReset);
        UART_sendString(msg);
    }
}

// UART Setup
void UART_SETUP() {
    P3SEL |= BIT3 + BIT4;       // Set RXD/TXD pins
    UCA0CTL1 |= UCSWRST;        // Software reset
    UCA0CTL0 = 0;               // Control register
    UCA0CTL1 |= UCSSEL_2;       // Use SMCLK

    UCA0BR0 = 0x09;             // Baud rate 115200
    UCA0BR1 = 0x00;
    UCA0MCTL = 0x02;

    UCA0CTL1 &= ~UCSWRST;       // Initialize UART state machine
}

// Send a character via UART
void UART_sendChar(char bip) {
    while (!(UCA0IFG & UCTXIFG));  // Wait for TX buffer to be ready
    UCA0TXBUF = bip;
}

// Receive a character via UART
char UART_getChar() {
    while (!(UCA0IFG & UCRXIFG));  // Wait for RX buffer to be ready
    return UCA0RXBUF;
}

// Send a string via UART
void UART_sendString(char* str) {
    while (*str) {
        UART_sendChar(*str++);
    }
}

// Receive a line of input via UART with backspace handling
void UART_getLine(char* buf, int limit) {
    int i = 0;
    char ch;

    while (1) {
        ch = UART_getChar();

        if (ch == '\r' || ch == '\n') {  // Enter key pressed
            buf[i] = '\0';  // Null-terminate the string
            UART_sendString("\r\n");
            break;
        } else if (ch == '\b' || ch == 127) {  // Backspace pressed
            if (i > 0) {
                i--;  // Move buffer pointer back
                UART_sendString("\b \b");  // Clear character on screen
            }
        } else if (i < limit - 1) {  // Normal character input
            buf[i++] = ch;
            UART_sendChar(ch);  // Echo the character back
        }
    }
}
