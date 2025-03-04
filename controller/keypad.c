#include "keypad.h"
#include "LEDarray.h"
#include "RGB.h"
#include "intrinsics.h"
#include <msp430.h>

// Keypad matrix mapping
static const char keymap[4][4] = {{'1', '2', '3', 'A'},
                                  {'4', '5', '6', 'B'},
                                  {'7', '8', '9', 'C'},
                                  {'*', '0', '#', 'D'}
                                  };

// Unlock code (can be changed)
static char unlock_code[4] = {'1', '2', '3', '4'};
static char entered_code[4];
static unsigned char code_index = 0;
static bool is_unlocked = false;
static system_state_t current_state = SYSTEM_LOCKED;

void keypad_init(void) {
  // Configure LED pin as output
  P1DIR |= LED_PIN;
  LED_PORT &= ~LED_PIN; // LED off initially

  rgb_init();

  // Configure row pins as outputs
  P4DIR |= ROW_PINS;
  P4OUT &= ~ROW_PINS; // Set rows low initially

  // Configure column pins as inputs with pull-up resistors
  P6DIR &= ~COL_PINS;
  P6REN |= COL_PINS; // Enable pull-up/down resistors
  P6OUT &= ~COL_PINS; // Set pull-down

  //rgb_set_color(0, 0, 0);
  return;
}

char keypad_scan(void) {
  char key = 0;
  int row, col;

  // Scan each row
  for (row = 0; row < 4; row++) {
    // Set current row high, others low
    ROW_PORT = (ROW1_PIN << row);

    // Small delay for signal to stabilize
    int i; 
    for (i = 0; i<7000; i++){}

    // Read columns
    unsigned int cols = (COL_IN) & 0x0F; // Read and invert (buttons pull low)

    if (cols) {               // If any column is low (button pressed)
      for (col = 0; col < 4; col++) {
        if (cols & (1 << col)) {
          key = keymap[row][col];
          while(cols !=0 ){
            cols = (COL_IN) & 0x0F;
          }
          break;
        }
      }
      break;
    }
  }

  // Return to default state
  ROW_PORT &= ~ROW_PINS;

  // If a key was pressed, process it for unlock code
  if (key && !is_unlocked) {
    rgb_set_color(0, 0, 1);
    entered_code[code_index] = key;
    code_index = (code_index + 1) % 5;
    // Check code when 4 digits entered
    if (code_index == 4) {
      code_index = 0;
      keypad_check_unlock();
    }
  }
  else if (key && is_unlocked){
    switch(key){
      case 'A':
        ledarray_decrease_period();
        break;
      case 'B':
        ledarray_increase_period();
        break;
      case 'D':
        entered_code[0] = 'G';
        is_unlocked=false;
        keypad_check_unlock();
        break;
      case '1':
        ledarray_select_pattern(PATTERN_1_TOGGLE);
        break;check_unlock();
        break;
      case '2':
        ledarray_select_pattern(PATTERN_2_UP_COUNT);
        break;check_unlock();
        break;
      case '3':
        ledarray_select_pattern(PATTERN_3_IN_OUT);
        break;check_unlock();
        break;
      case '0':
        ledarray_select_pattern(PATTERN_0_STATIC);
        break;
    }
  }

  return key;
}

bool keypad_check_unlock(void) {
  int i;
  bool match = true;

  for (i = 0; i < 4; i++) {
    if (entered_code[i] != unlock_code[i]) {
      match = false;
    }
  }
  is_unlocked = match;
  if (is_unlocked) {
    current_state = SYSTEM_UNLOCKED;
    rgb_set_color(0, 1, 0);
  } else {
    current_state = SYSTEM_LOCKED;
    rgb_set_color(1, 0, 0);
  }

  return match;
}

bool keypad_is_unlocked(void) { return is_unlocked; }
