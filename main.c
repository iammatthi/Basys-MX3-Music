#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <p32xxxx.h>
#include <time.h>
#include <stdlib.h>

#include "General.h"
#include "Speaker.h"
#include "Timer.h"
#include "LedRGB.h"
#include "LCD.h"

// Disable JTAG to use RA0
#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF
// Device Config Bits in  DEVCFG1:
#pragma config FNOSC = FRCPLL
#pragma config FSOSCEN = OFF
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = ON
#pragma config FPBDIV = DIV_4
// Device Config Bits in  DEVCFG2:
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_2

void __attribute__((interrupt(IPL2AUTO), vector(4))) Timer1_Handler() {
    IFS0bits.T1IF = 0;
}

int flagTimer2 = 0;

void __attribute__((interrupt(IPL3AUTO), vector(8))) Timer2_Handler() {
    flagTimer2 = 1;
    IFS0bits.T2IF = 0;
}

void main() {
    srand(time(NULL));
    
    // Set all IO pins to digital
    ALL_DIGITAL_IO();
    // Enable multi-vector interrupts
    ENABLE_MULTIVECTOR_INTERRUPTS();
    // Configure Timer1 (for speaker)
    Timer1_Configure(1);
    // Configure Timer2 (for timeout)
    Timer2_Configure(1);
    // Configure Led RGB
    LedRGB_Configure();
    // Configure Speaker
    Speaker_Configure();
    // Configure LCD
    LCD_Configure();
    LCD_Clear();

    LCD_PutString("github:iammatthi");
    char song1[RICK_ROLL_LENGTH] = RICK_ROLL_NOTES;
    int song1Tempo = RICK_ROLL_TEMPO;
    char song2[L_AMOUR_TOUJOURS_LENGTH] = L_AMOUR_TOUJOURS_NOTES;
    int song2Tempo = L_AMOUR_TOUJOURS_TEMPO;
    char song3[PIRATE_LENGTH] = PIRATE_NOTES;
    int song3Tempo = PIRATE_TEMPO;
    char song4[EINE_KLEINE_NACHTMUSIK_LENGTH] = EINE_KLEINE_NACHTMUSIK_NOTES;
    int song4Tempo = EINE_KLEINE_NACHTMUSIK_TEMPO;
    
    
    LCD_Cmd(0x80 | 0x40); // Set DDRAM Address to 0x40 (start of second line)
    LCD_PutString("song 1");
    Speaker_Play(song1, song1Tempo);
//    LCD_Cmd(0x80 | 0x40); // Set DDRAM Address to 0x40 (start of second line)
//    LCD_PutString("song 2");
//    Speaker_Play(song2, song2Tempo);
//    LCD_Cmd(0x80 | 0x40); // Set DDRAM Address to 0x40 (start of second line)
//    LCD_PutString("song 3");
//    Speaker_Play(song3, song3Tempo);
//    LCD_Cmd(0x80 | 0x40); // Set DDRAM Address to 0x40 (start of second line)
//    LCD_PutString("song 4");
//    Speaker_Play(song4, song4Tempo);
    LCD_Cmd(0x80 | 0x40); // Set DDRAM Address to 0x40 (start of second line)
    LCD_PutString("the end");
}