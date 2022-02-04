#include <p32xxxx.h>
#include <stdlib.h>

// configurazione, utilizza timer2

void LedRGB_Configure() {
    RPD2R = 0x0B; // OC3 in RD2 (R) -> led RGB
    RPD12R = 0x0B; // OC5 in RD12 (G) -> led RGB
    RPD3R = 0x0B; // OC4 in RD3 (B) -> led RGB

    OC3CONbits.OCM = 0b110; // PWM mode senza fault pin
    OC4CONbits.OCM = 0b110; // PWM mode senza fault pin
    OC5CONbits.OCM = 0b110; // PWM mode senza fault pin

    OC3RS = 0;
    OC3R = 0;

    OC5RS = 0;
    OC5R = 0;

    OC4RS = 0;
    OC4R = 0;

    OC3CONbits.ON = 1;
    OC4CONbits.ON = 1;
    OC5CONbits.ON = 1;
}

// set con valori RGB scalati da 0 a 255, utilizza timer 2

void LedRGB_Set(unsigned char red, unsigned char green, unsigned char blue) {
    int maxRange = PR2;
    // imposta la percentuale del colore cambiando il duty cycle di ogni OC
    OC3RS = maxRange * red / 255;
    OC5RS = maxRange * green / 255;
    OC4RS = maxRange * blue / 255;
}

void LedRGB_Random() {
    LedRGB_Set((double) rand() / RAND_MAX * 50, (double) rand() / RAND_MAX * 50, (double) rand() / RAND_MAX * 50);
}

void LedRGB_Off() {
    LedRGB_Set(0, 0, 0);
}

// RGB test
// LedRGB_Set(89, 203, 232);
