#ifndef RGBH
#define RGBH

#include <msp430.h>

// RGB LED pins (using Timer_B PWM)
#define RGB_RED_PIN   BIT7    // P1.7 - TB1.1
#define RGB_GREEN_PIN BIT6    // P1.6 - TB1.2
#define RGB_BLUE_PIN  BIT5    // P1.5 - TB1.3

void rgb_init();
void rgb_set_color(unsigned char red, unsigned char green, unsigned char blue);

#endif