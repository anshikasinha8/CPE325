/*-------------------------------------------------------------------------
 * File:        Lab10_P1_P2.c
 * Function:    Sample 3D Accelerometer
 * Description: This C program interfaces an accelerometer, with the MSP430F5529. 
                It reads acceleration values from the X, Y, and Z axes, converts them 
                into digital values using the ADC12 module, and sends the data over 
                UART to the UAH Serial App.
 * Input:       Accelerometer signal
 * Output:      P1: X, Y, Z sample lines on UAH Serial App
 *              P2: RED LED is on when |acceleration| reaches 3g, otherwise
                    GREEN LED is on.
 * Author(s):   Anshika Sinha
 * Date:        03/25/25
 *-----------------------------------------------------------------------*/
#include <msp430.h>
#include <math.h>

#define ADC_TO_G(n) ((3.0 * n / 4095 - 1.5) / 0.3) //convert to g

volatile long int ADCXval, ADCYval, ADCZval;
volatile float Xper, Yper, Zper;
volatile float M;

void TimerA_setup(void)
{
    TA0CCTL0 = CCIE;                     // Enabled interrupt
    TA0CCR0 = 3276;                      // 3277 / 32768 = .1s for ACLK
    TA0CTL = TASSEL_1 + MC_1;            // ACLK, up mode
    P1DIR |= BIT0;                       // Setup LED1 at P2.1
    P4DIR |= BIT7;                       // Setup LED2 at P4.7
}

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

void UART_putCharacter(char c) {
    while (!(UCA0IFG&UCTXIFG));    // Wait for previous character to transmit
    UCA0TXBUF = c;                 // Put character into tx buffer
}

void UART_setup(void) {
    P3SEL |= BIT3 + BIT4;               // Set up Rx and Tx bits
    UCA0CTL0 = 0;                       // Set up default RS-232 protocol
    UCA0CTL1 |= BIT0 + UCSSEL_2;        // Disable device, set clock
    UCA0BR0 = 27;                       // 1048576 Hz / 38400
    UCA0BR1 = 0;
    UCA0MCTL = 0x94;
    UCA0CTL1 &= ~BIT0;                  // Start UART device
}

void sendData(void)
{
    int i;

    // Part 1 - get samples from ADC
    Xper = (ADC_TO_G(ADCXval));    // Calculate percentage outputs
    Yper = (ADC_TO_G(ADCYval));    // Calculate percentage outputs
    Zper = (ADC_TO_G(ADCZval));    // Calculate percentage outputs

    // Use character pointers to send one byte at a time
    char *xpointer=(char *)&Xper;
    char *ypointer=(char *)&Yper;
    char *zpointer=(char *)&Zper;

    UART_putCharacter(0x55);            // Send header
    for(i = 0; i < 4; i++)
    {        
        UART_putCharacter(xpointer[i]); // Send x percentage - one byte at a time
    }
    for(i = 0; i < 4; i++)
    {            
        UART_putCharacter(ypointer[i]);// Send y percentage - one byte at a time
    }
    for(i = 0; i < 4; i++)
    {            
        UART_putCharacter(zpointer[i]);// Send z percentage - one byte at a time
    }
}

//******************************************************************************
// MAIN
//******************************************************************************
void main (void)
{
    WDTCTL = WDTPW +WDTHOLD;            // Stop WDT

    TimerA_setup();                     // Setup timer to send ADC data
    ADC_setup();                        // Setup ADC
    UART_setup();                       // Setup UART for RS-232
    _EINT();

    while (1){
        ADC12CTL0 |= ADC12SC;               // Start conversions
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0
    }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void) {
    ADC12IFG = 0x00;
    ADCXval = ADC12MEM0;          // Move results, IFG is cleared
    ADCYval = ADC12MEM1;
    ADCZval = ADC12MEM2;
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA_isr() {
    // Calculate magnitude of acceleration
    M = sqrt((Xper * Xper) + (Yper * Yper) + (Zper * Zper)); 

    if (M >= 3) 
    {
        P1OUT |= BIT0;         // Turn on RED LED
        P4OUT &= ~BIT7;        // Turn off GREEN lED
    }
    else 
    {
        P1OUT &= ~BIT0;        // Turn off RED LED
        P4OUT |= BIT7;         // Turn on GREEN LED
    }
    sendData();                           // Send data to serial app
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}
