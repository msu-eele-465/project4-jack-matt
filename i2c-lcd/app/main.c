#include "intrinsics.h"
#include "msp430fr2310.h"
#include "heartbeat.h"
#include "LCD_driver.h"

void enable(){
  P1OUT |= BIT0;
  __delay_cycles(1000);
  P1OUT &= ~BIT0;
}


void main(){
  WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer
  // Disable low-power mode / GPIO high-impedance
  PM5CTL0 &= ~LOCKLPM5;
  heartbeat_init();
  init_lcd();



  lcd_send_command(LCD_CLEAR_DISPLAY);
  // lcd_send_command(LCD_CLEAR_DISPLAY);
  // init_lcd();
  lcd_send_command(LCD_CURSOR_BLINK);

  lcd_send_string("Hi Jack/Matt");

  while (1){
  }
}