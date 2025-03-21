
#include <msp430.h>
#include "RGB.h"

void rgb_init(){
  // Configure RGB LED pins for PWM
  P3DIR |= RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN;      // setting ouput for PWM
  P3OUT &= ~(RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);   // clearing out (initially off)
  return;
}

void rgb_set_color(unsigned char red, unsigned char green, unsigned char blue) {
  if(red>blue && red>green){
    P3OUT &= ~(RGB_GREEN_PIN | RGB_BLUE_PIN);
    P3OUT |= RGB_RED_PIN;
  } else if(blue>red && blue>green){
    P3OUT &= ~(RGB_RED_PIN | RGB_GREEN_PIN);
    P3OUT |= RGB_BLUE_PIN;
  } else if(green>red && green>blue){
    P3OUT &= ~(RGB_RED_PIN | RGB_BLUE_PIN);
    P3OUT |= RGB_GREEN_PIN;
  } else if (red == 255){
    P3OUT |= (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);
  } else {
    P3OUT &= ~(RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);
  }
  return;
}
