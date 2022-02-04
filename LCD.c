#include <p32xxxx.h>
#include "Timer.h"
#include "LCD.h"

void LCD_Configure() {
    TRISE = 0x00FF; // RE0:7 as digital input , or 0x0000 as out is the same
    TRISDbits.TRISD4 = 0; // RD4 as digital output ENpin
    TRISDbits.TRISD5 = 0; // RD5 as digital output RWpin
    TRISBbits.TRISB15 = 0;  // RB15 as digital output RSpin

    // PMP initialization
    PMCON = 0x83BF; // Enable the PMP, long waits
    PMMODE = 0x3FF; // Master Mode 1
    PMAEN = 0x0001; // PMA0 enabled

    PMADDR = LCDCMD; // command register (ADDR = 0)
    Timer2_DelayMs(1); // > something
    PMDATA = 0x38; // 8-bit interface, 2 lines, 5x8
    Timer2_DelayMs(1); // > 48 us
    PMDATA = 0x0c; // ON, no cursor, no blink
    Timer2_DelayMs(1); // > 48 us
    PMDATA = 0x01; // clear display
    Timer2_DelayMs(2); // > 1.6 ms
    PMDATA = 0x06; // increment cursor, no shift
    Timer2_DelayMs(2); // > 1.6 ms
}

char LCD_Read(int addr) {
    int dummy;
    while (PMMODEbits.BUSY); // wait for PMP available
    PMADDR = addr; // select the command address
    dummy = PMDATA; // init read cycle, dummy read
    while (PMMODEbits.BUSY); // wait for PMP to be available
    return PMDATA; // read the status register
}

void LCD_Write(int addr, char c) {
    while (LCD_Busy()); // check busy flag of LCD
    while (PMMODEbits.BUSY); // wait for PMP available
    PMADDR = addr;
    PMDATA = c;
}

void LCD_PutString(char *s) {
    while (*s) {
        LCD_Put(*s++);
    }
}