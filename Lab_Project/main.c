/*-------------------------------------------------------------------------
 * File:        main.c
 * Function:    Digital Level
 * Description: This project implements a digital level using the MSP430F5529 
            microcontroller and a 3-axis accelerometer (ADXL335). It measures tilt 
            angles in real-time and provides visual and serial output feedback.
 * Input:       Accelerometer signal
 * Output:      LEDs and serial output on UAH Serial App
 * Author:      Anshika Sinha
 * Date:        04.15.2025
 *-----------------------------------------------------------------------*/
// ---------- Include libraries -------------------------------------------
#include "intrinsics.h"
#include "msp430f5529.h"
#include <msp430.h>
#include <math.h>

// ---------- Definitions and global variables ----------------------------
#define ADC_TO_G(n) ((3.0 * n / 4095 - 1.5) / 0.3) //convert to g

volatile long int ADCXval, ADCYval, ADCZval;
volatile float Xper, Yper, Zper, xTilt, yTilt;
volatile float xZero, yZero, zZero, baseX, baseY;

// ----------- Function prototypes ----------------------------------------
void TimerA_setup(void);
void ADC_setup(void);
void UART_setup(void);
void UART_putCharacter(char c);
void Switch_setup(void);
void readADC(void);
void sendData(void);
void calibrate(void);

//******************************************************************************
// MAIN
//******************************************************************************
void main (void)
{
    WDTCTL = WDTPW + WDTHOLD;            // Stop WDT

    TimerA_setup();                     // Setup timer to send ADC data
    ADC_setup();                        // Setup ADC
    UART_setup();                       // Setup UART for RS-232
    Switch_setup();                     // Setup switch for calibration
    _EINT();                            // Enable interrupts

    while (1){
        ADC12CTL0 |= ADC12SC;               // Start conversions
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0
    }
}

// ----------- Setup Timer A ----------------------------------------------
void TimerA_setup(void) {
    TA0CCTL0 = CCIE;                     // Enabled interrupt
    TA0CCR0 = 3276;                      // 3277 / 32768 = .1s for ACLK
    TA0CTL = TASSEL_1 + MC_1;            // ACLK, up mode
    P1DIR |= BIT0;                       // Setup LED1 at P2.1
    P4DIR |= BIT7;                       // Setup LED2 at P4.7
}

// ----------- Setup Accelerometer ADC convertor ---------------------------
void ADC_setup(void) {
    int i =0;

    P6DIR &= ~(BIT3 + BIT4 + BIT5);         // Configure P6.3, P6.4, P6.5 as input pins
    P6SEL |= BIT3 + BIT4 + BIT5;            // Configure P6.3, P6.4, P6.5 as analog pins
    // configure ADC converter
    ADC12CTL0 = ADC12ON + ADC12SHT0_6 + ADC12MSC;
    ADC12CTL1 = ADC12SHP + ADC12CONSEQ_3;   // Use sample timer, single sequence
    ADC12MCTL0 = ADC12INCH_3;               // ADC A3 pin -X-axis
    ADC12MCTL1 = ADC12INCH_4;               // ADC A4 pin - Y-axis
    ADC12MCTL2 = ADC12INCH_5 + ADC12EOS;    // ADC A5 pin - Z-axis

                                        // EOS - End of Sequence for Conversions
    ADC12IE = ADC12IE0;                 // Enable ADC12IFG.1
    for (i = 0; i < 0x3600; i++);       // Delay for reference start-up
    ADC12CTL0 |= ADC12ENC;              // Enable conversions
}

// ----------- Setup UART at 38400 Baud rate --------------------------------
void UART_setup(void) {
    P3SEL |= BIT3 + BIT4;               // Set up Rx and Tx bits
    UCA0CTL0 = 0;                       // Set up default RS-232 protocol
    UCA0CTL1 |= BIT0 + UCSSEL_2;        // Disable device, set clock
    UCA0BR0 = 27;                       // 1048576 Hz / 38400
    UCA0BR1 = 0;
    UCA0MCTL = 0x94;
    UCA0CTL1 &= ~BIT0;                  // Start UART device
}

// ----------- Send message through UART ------------------------------------
void UART_putCharacter(char c) {
    while (!(UCA0IFG&UCTXIFG));    // Wait for previous character to transmit
    UCA0TXBUF = c;                 // Put character into tx buffer
}

// ----------- Setup switch for calibration ---------------------------------
void Switch_setup(void) {
    P1DIR &= ~BIT1;                 // Set P1.1 as input for Switch input
    P1REN |= BIT1;                  // Enable pull-up register at P1.1
    P1OUT |= BIT1;                  

    P1IE |= BIT1;                   // Enable interrupt for Switch (P1.1)
    P1IES |= BIT1;                  // Set interrupt from high to low
    P1IFG &= ~BIT1;                 // Clear interrupt flag
}

// ----------- Read ADC values from accelerometer ---------------------------
void readADC(void) {
    ADCXval = ADC12MEM0;            // Move results to x, y, z respectively
    ADCYval = ADC12MEM1;
    ADCZval = ADC12MEM2;
}

// -- Converts ADC values, calculates tilt angles, and transmits characters --
void sendData(void) {
    unsigned int i;

    // Get samples from ADC
    Xper = (ADC_TO_G(ADCXval));    // Calculate percentage outputs
    Yper = (ADC_TO_G(ADCYval));    // Calculate percentage outputs
    Zper = (ADC_TO_G(ADCZval));    // Calculate percentage outputs

    // Calculate angles using arctangent formulas
    xTilt = abs(atan2f(Xper, sqrtf(Yper*Yper + Zper*Zper)) * (180.0f / M_PI)) - baseX;
    yTilt = abs(atan2f(Yper, sqrtf(Xper*Xper + Zper*Zper)) * (180.0f / M_PI)) - baseY;
    
    // Use character pointers to send one byte at a time
    char *xpointer=(char *)&xTilt;
    char *ypointer=(char *)&yTilt;

    UART_putCharacter(0x55);            // Send header
    for(i = 0; i < 4; i++)
    {        
        UART_putCharacter(xpointer[i]); // Send x percentage - one byte at a time
    }
    for(i = 0; i < 4; i++)
    {            
        UART_putCharacter(ypointer[i]);// Send y percentage - one byte at a time
    }
}

// ----------- Calibrate to current position ----------------------------------
void calibrate(void) {
    float xSum, ySum, zSum;
    unsigned int i = 0;

    // Blink LEDs to indicate calibration
    P1OUT ^= BIT0;                  // Toggle RED LED
    P4OUT ^= BIT7;                  // Toggle GREEN LED
    __delay_cycles(32768);           // 1 s delay

    // Get samples from ADC
    for (i = 0; i < 20; i++) {
        readADC();
        xSum += (ADC_TO_G(ADCXval));    // Calculate percentage outputs
        ySum += (ADC_TO_G(ADCYval));    // Calculate percentage outputs
        zSum += (ADC_TO_G(ADCZval));    // Calculate percentage outputs
        __delay_cycles(3276);           // 100ms between samples
    }

    // Set zero positions
    xZero = xSum / 20;
    yZero = ySum / 20;
    zZero = zSum / 20;

    baseX = atan2f(xZero, sqrtf(yZero*yZero + zZero*zZero)) * (180.0f / M_PI);
    baseY = atan2f(yZero, sqrtf(xZero*xZero + zZero*zZero)) * (180.0f / M_PI);

    // Turn off LEDs
    P1OUT &= ~BIT0;                  
    P4OUT &= ~BIT7;
}

//******************************************************************************
// INTERRUPTS
//******************************************************************************
#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void) {
    ADC12IFG = 0x00;                // Clear IFG
    readADC();
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA_isr() {
    if (xTilt <= 5 && yTilt <= 5) 
    {
        P1OUT &= ~BIT0;        // Turn off RED LED
        P4OUT |= BIT7;         // Turn on GREEN LED
        
    }
    else if (xTilt <= 10 && yTilt <= 10) // If tilt > 5 && <= 10
    {
        P1OUT ^= BIT0;         // Toggle RED LED
    }
    else                        // Tilt > 10
    {
        P1OUT |= BIT0;         // Turn on RED LED
        P4OUT &= ~BIT7;        // Turn off GREEN lED
    }
    sendData();                           // Send data to serial app
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    __delay_cycles(40000);      // Debounce
    P1IE &= ~BIT1;              // Disable interrupt

    if ((P1IN&BIT1) == 0){      // If switch is still pressed
        calibrate();            // Start calibration
    } 

    P1IFG &= ~BIT1;             // Clear interrupt flag register
    P1IE |= BIT1;               // Enable interrupt
}
