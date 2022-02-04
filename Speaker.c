#include <p32xxxx.h>
#include <string.h>
#include <ctype.h>
#include "General.h"
#include "Timer.h"
#include "Speaker.h"
#include "Utils.h"
#include "LedRGB.h"

// Store all note frequencies for each octave in a 2 dimensional array
double octaves[7][7] = {
    // Octave1 Notes: c1, d1, e1, f1, g1, a1, b1
    {32.703, 36.708, 41.203, 43.654, 48.990, 55.000, 61.735},
    // Octave2 Notes: c2, d2, e2, f2, g2, a2, b2
    {65.406, 73.416, 82.407, 87.307, 97.999, 110.000, 123.470},
    // Octave3 Notes: c3, d3, e3, f3, g3, a3, b3
    {130.810, 146.830, 164.810, 174.610, 196.000, 220.000, 246.940},
    // Octave4 Notes: c4, d4, e4, f4, g4, a4, b4
    {261.630, 293.660, 329.630, 349.230, 392.000, 440.000, 493.880},
    // Octave5 Notes: c5, d5, e5, f5, g5, a5, b5
    {523.250, 587.330, 659.250, 698.460, 783.990, 880.000, 987.770},
    // Octave6 Notes: c6, d6, e6, f6, g6, a6, b6
    {1046.500, 1174.700, 1318.500, 1396.900, 1568.000, 1760.000, 1979.500},
    // Octave7 Notes: c7, d7, e7, f7, g7, a7, b7
    {2093.000, 2349.300, 2637.000, 2793.800, 3136.000, 3520.000, 3951.100}
};

// Store the sharp note frequencies for each octave in a 2 dimensional array
double sharpOctaves[7][5] = {
    // Octave1 Sharps: C1, D1, F1, G1, A1
    {34.648, 38.891, 46.249, 51.913, 58.270},
    // Octave2 Sharps: C2, D2, F2, G2, A2
    {69.296, 77.782, 92.499, 103.830, 116.540},
    // Octave3 Sharps: C3, D3, F3, G3, A3
    {138.590, 155.560, 185.000, 207.650, 233.080},
    // Octave4 Sharps: C4, D4, F4, G4, A4
    {227.180, 311.130, 369.990, 415.300, 466.160},
    // Octave5 Sharps: C5, D5, F5, G5, A5
    {554.370, 622.250, 739.990, 830.610, 932.330},
    // Octave6 Sharps: C6, D6, F6, G6, A6
    {1108.700, 1244.500, 1480.000, 1661.200, 1864.700},
    // Octave7 Sharps: C7, D7, F7, G7, A7
    {2217.500, 2489.000, 2960.000, 3322.400, 3729.300}
};

// whole, 1/2, 1/4, 1/8, 1/16, 1/32
double noteLengths[] = {1, 0.5, 0.25, 0.125, 0.0625, 0.03125};

void __attribute__((interrupt(IPL1AUTO), vector(12))) Timer3_Handler() {
    // why is this mandatory?
    IFS0bits.T3IF = 0; // clear Timer3 interrupt flag
}

void Speaker_Configure() {
    TRISBbits.TRISB14 = 0; // Port B14 as output
    RPB14R = 0b1100; // Map OC1 to B14

    T3CONbits.ON = 0;
    T3CONbits.TCKPS = 0b000; // prescaler 1
    T3CONbits.TGATE = 0; // not gated input
    T3CONbits.TCS = 0; // PBCLK input

    OC1CONbits.ON = 0; // Turn off OC1 while doing setup.
    OC1CONbits.OCM = 6; // PWM mode on OC1; Fault pin is disabled
    OC1CONbits.OCTSEL = 1; // Timer3 is the clock source for this Output Compare module

    IPC3bits.T3IP = 1; // interrupt priority
    IPC3bits.T3IS = 0; // interrupt subpriority
    IFS0bits.T3IF = 0; // clear Timer3 interrupt flag
    IEC0bits.T3IE = 1; // enable Timer3 interrupt
}

void Speaker_PlayFreq(int freqHz, int durationMs) {
    PR3 = -1 + PBCLK_FREQ / freqHz; // -1 + PBCLK / (pwmFreq * T3_PRESC)
    TMR3 = 0; // reset timer3
    T3CONbits.ON = 1;

    OC1RS = (PR3 + 1) / 2; // half period square wave (not sure is correct)
    OC1CONbits.ON = 1; // Start the OC1 module
    
    LedRGB_Random();

    Timer2_DelayMs(durationMs);
    Speaker_Stop();
    LedRGB_Off();
}

void Speaker_Stop() {
    T3CONbits.ON = 0; // Turn off Timer3
    OC1CONbits.ON = 0; // Turn off OC1
}

void Speaker_Play(char *notes, unsigned int tempo) {
    // remove spaces from song
    char *cleanNotes = removeWhiteSpace(notes);

    for (int i = 0; i < strlen(cleanNotes); i += 3) {
        // get the current note to be played
        char noteLetter = cleanNotes[i];

        // get octave number of note to be played
        int noteOctave = charToDigit(cleanNotes[i + 1]);

        // get note length
        int noteLength = charToDigit(cleanNotes[i + 2]);
        // calculate the notes length in milliseconds
        int noteLengthMs = Speaker_NoteLengthToMs(noteLengths[noteLength], tempo);

        Speaker_PlayNote(noteLetter, noteOctave, noteLengthMs);
    }
}

int Speaker_NoteLengthToMs(double noteLen, int tempo) {
    // Convert tempo from notes per minute to notes per second
    double notesPerSecond = tempo / 60.0;

    // Calculate the length of one whole note in milliseconds
    int wholeNoteMs = 1000 / notesPerSecond;

    // Calculate the length of note (whole, 1/2, 1/4, etc.) in milliseconds
    return wholeNoteMs * noteLen;
}

void Speaker_PlayNote(char note, int noteOctaveNum, int noteLengthMs) {
    if (note == '-') {
        // Play nothing
        Timer2_DelayMs(noteLengthMs);
        return;
    }

    // Convert the note letter to an array index
    int noteFreqIndex;
    // Convert octave number to an array index
    int octaveIndex = noteOctaveNum - 1;
    double freq;
    if (isupper(note)) {
        noteFreqIndex = Speaker_SharpToIndex(note);
        freq = sharpOctaves[octaveIndex][noteFreqIndex];
    } else {
        noteFreqIndex = Speaker_NoteToIndex(note);
        freq = octaves[octaveIndex][noteFreqIndex];
    }

    Speaker_PlayFreq(freq, noteLengthMs);
}

int Speaker_SharpToIndex(char note) {
    switch (note) {
        case 'C':
            return 0;
            break;
        case 'D':
            return 1;
            break;
        case 'F':
            return 2;
            break;
        case 'G':
            return 3;
            break;
        case 'A':
            return 4;
            break;
        default:
            // Plays a error tone if none of the note letters match
            Speaker_PlayFreq(100, 1000);
    }
}

int Speaker_NoteToIndex(char note) {
    switch (note) {
        case 'c':
            return 0;
            break;
        case 'd':
            return 1;
            break;
        case 'e':
            return 2;
            break;
        case 'f':
            return 3;
            break;
        case 'g':
            return 4;
            break;
        case 'a':
            return 5;
            break;
        case 'b':
            return 6;
            break;
        default:
            // Plays a error tone if none of the note letters match
            Speaker_PlayFreq(100, 1000);
    }
}

// Speaker example
// Speaker_PlayFreq(100, 3000); // 100Hz for 3000ms
// char song[RICK_ROLL_LENGTH] = RICK_ROLL_NOTES;
// Speaker_Play(song, RICK_ROLL_TEMPO);
