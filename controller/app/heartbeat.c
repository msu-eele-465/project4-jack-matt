#include <msp430.h>
#include "heartbeat.h"

void heartbeat_init(void) {
  P1DIR |= BIT6; // configure LED2
  P1OUT &= ~BIT6;


  //-- SETUP TIMER -------------------------------------------------------
  TB1CTL |= TBCLR;
  TB1CTL |= TBSSEL__SMCLK;     // Source SMCLK
  TB1CTL |= MC__UP;
  TB1CTL |= CNTL_0;

  // 15/513 = 0.029239766
  // 0.029239766÷(0.000001×7) = 4177.109428571
  // 2.5/513 = 0.004873294
  // 0.004873294/(.000001*7) = 696.184857143
  TB1CCR0 = 65535;

  //TB1CTL |= ID__4;          // setting divider D1
  TB1EX0 |= TBIDEX__7;        // setting divider D2

  TB1CCTL0 |= CCIE;           // Clear ISR flag
  TB1CCTL0 &= ~CCIFG;         // Enable timer interrupt

  __enable_interrupt();       // Enable Maskable IRQs

  return;
}

void heartbeat_run(void) {
  // flip the P6.6 bit to alternate the LED on/off
  P1OUT ^= BIT6;
  return;
}

#pragma vector = TIMER1_B0_VECTOR
__interrupt void heartbeat_timer(void)
{
    heartbeat_run();
    TB0CTL &= ~TBIFG;
    return;
}