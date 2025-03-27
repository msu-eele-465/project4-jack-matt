#include "LCD_driver.h"
#include "intrinsics.h"
#include "msp430fr2310.h"

uint8_t cursor_on = 1; // initialize cursor on
uint8_t blink_on = 0;  // initialize blink off

void init_lcd() {
  P1DIR |=
      BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7; // EN, RS, DB4, DB5, DB6, DB7
  P1OUT &= ~(BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7); // Clear outputs

  __delay_cycles(50000);
  lcd_set_function();
  __delay_cycles(10000);
  lcd_send_command(LCD_DISPLAY_ON);
  __delay_cycles(10000);
  lcd_send_command(LCD_ENTRY_MODE_SET);
  __delay_cycles(10000);
  lcd_send_command(LCD_CURSOR_ON);
  __delay_cycles(10000);
  lcd_send_command(LCD_CLEAR_DISPLAY);
  lcd_clear_bottom();
  __delay_cycles(10000);
  lcd_clear_top();
  __delay_cycles(10000);
  lcd_send_command(LCD_CURSOR_LEFT);
  __delay_cycles(10000);
  lcd_send_command(LCD_CLEAR_DISPLAY);
  __delay_cycles(10000);
}

void lcd_send_command(uint8_t cmd) {
  P1OUT &= ~BIT1;       // RS = 0 for command
  P1OUT = (cmd & 0xF0); // Send high nibble
  P1OUT |= BIT0;        // Enable pulse
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  __delay_cycles(1000);
  P1OUT = ((cmd << 4) & 0xF0); // Send low nibble
  P1OUT |= BIT0;
  __delay_cycles(1000);
  P1OUT &= ~BIT0;
  __delay_cycles(1000);
}

void lcd_send_data(uint8_t data) {
  P1OUT = (data & 0xF0); // Send high nibble
  P1OUT |= BIT1;         // RS = 1 for data
  P1OUT |= BIT0;         // Enable pulse
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  __delay_cycles(1000);
  P1OUT = ((data << 4) & 0xF0) | BIT1; // Send low nibble
  P1OUT |= BIT0;
  __delay_cycles(1000);
  P1OUT &= ~BIT0;
  __delay_cycles(1000);
}

void lcd_send_string(char *str) {
  while (*str != '\0') {
    lcd_send_data(*str++);
  }
}

void lcd_set_key_char(uint8_t data) {
  lcd_send_command(0x80 | 0x4F); // set DDRAM to bottom right corner
  __delay_cycles(1000);
  lcd_send_data(data);
}

void lcd_toggle_cursor() {
  if (cursor_on) {
    lcd_send_command(LCD_DISPLAY_ON); // Display ON, Cursor OFF
  } else {
    lcd_send_command(LCD_CURSOR_ON); // Display ON, Cursor ON
  }

  cursor_on = !cursor_on;
}

void lcd_toggle_blink() {
  if (blink_on) {
    lcd_send_command(LCD_CURSOR_ON); // Display ON, Cursor ON, Blink OFF
  } else {
    lcd_send_command(LCD_CURSOR_BLINK); // Display ON, Cursor ON, Blink ON
  }
  __delay_cycles(10000);
  lcd_send_command(LCD_RETURN_HOME);
  blink_on = !blink_on;
}

void lcd_set_function() {
  P1OUT &= ~BIT1;      // RS = 0 for command
  P1OUT = (0x03 << 4); // Send high nibble
  P1OUT |= BIT0;       // Enable pulse
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  __delay_cycles(30000);

  P1OUT = (0x03 << 4); // Send high nibble
  P1OUT |= BIT0;       // Enable pulse
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  __delay_cycles(30000);

  P1OUT = (0x03 << 4); // Send high nibble
  P1OUT |= BIT0;       // Enable pulse
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  __delay_cycles(30000);

  P1OUT = (0x02 << 4); // Send low nibble
  P1OUT |= BIT0;
  __delay_cycles(1000);
  P1OUT &= ~BIT0;

  lcd_send_command(LCD_FUNCTION);
}

void lcd_clear_top() {
  lcd_send_command(LCD_RETURN_HOME);
  __delay_cycles(1000);
  int i = 0;
  for (i = 0; i < 16; i++) {
    lcd_send_data(' ');
    __delay_cycles(1000);
  }
  lcd_send_command(LCD_RETURN_HOME);
}

void lcd_clear_bottom() {
  lcd_send_command(LCD_BOTTOM_LINE);
  __delay_cycles(1000);
  int i = 0;
  for (i = 0; i < 16; i++) {
    lcd_send_data(' ');
    __delay_cycles(1000);
  }
  lcd_send_command(LCD_BOTTOM_LINE);
}