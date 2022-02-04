#include <p32xxxx.h>
#include "General.h"

extern int flagTimer2;

void Timer1_Configure(unsigned int ms) {
    T1CONbits.ON = 0;

    T3CONbits.TCKPS = 0b000; // prescaler 1
    T3CONbits.TGATE = 0; // not gated input
    T3CONbits.TCS = 0; // PBCLK input
    PR2 = ms * PBCLK_FREQ / 1000;

    // Set interrupt register
    IPC1bits.T1IP = 2; // priority
    IPC1bits.T1IS = 0; // sub-priority
    IFS0bits.T1IF = 0; // Reset T1 interrupt flag
    IEC0bits.T1IE = 1; //interrupt T1 enable

    TMR1 = 0; // reset T1
    T1CONbits.ON = 1;
}

void Timer2_Configure(unsigned int ms) {
    T2CONbits.ON = 0;

    T3CONbits.TCKPS = 0b000; // prescaler 1
    T3CONbits.TGATE = 0; // not gated input
    T3CONbits.TCS = 0; // PBCLK input
    T2CONbits.T32 = 0; // use 16 - bit timer
    PR2 = ms * PBCLK_FREQ / 1000;

    // Set interrupt register
    IPC2bits.T2IP = 3; // priority
    IPC2bits.T2IS = 0; // sub-priority
    IFS0bits.T2IF = 0; // Reset T2 interrupt flag
    IEC0bits.T2IE = 1; //interrupt T2 enable

    TMR2 = 0; // reset T2
    T2CONbits.ON = 1;
}

void Timer2_DelayMs(unsigned int ms) {
    int count = 0;
    while (count != ms) {
        if (flagTimer2) {
            count++;
            flagTimer2 = 0;
        }
    }
}
