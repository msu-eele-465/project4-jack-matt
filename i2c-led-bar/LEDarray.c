#include "LEDarray.h"
#include <msp430.h>

// Pattern timing control
static float base_transition_period = 1.0;  // Default 1.0s
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
static led_pattern_t current_pattern = PATTERN_NONE;
static bool pattern_active = false;

// Pattern definitions
static const unsigned char STATIC_PATTERN = 0xAA;  // 10101010

static const unsigned char TOGGLE_PATTERN[2] = {
    0xAA,   // 10101010
    0x55    // 01010101
};

static const unsigned char IN_OUT_PATTERN[6] = {
    0x18,   // 00011000
    0x24,   // 00100100
    0x42,   // 01000010
    0x81,   // 10000001
    0x42,   // 01000010
    0x24    // 00100100
};

// Timer initialization for pattern updates
static void init_timer(void) {
    //-- SETUP TIMER -------------------------------------------------------
    TB0CTL |= TBCLR;
    TB0CTL |= TBSSEL__SMCLK;     // Source SMCLK
    TB0CTL |= MC__UP;
    TB0CTL |= CNTL_0;

    // 15/513 = 0.029239766
    // 0.029239766÷(0.000001×7) = 4177.109428571
    // 2.5/513 = 0.004873294
    // 0.004873294/(.000001*7) = 696.184857143
    TB0CCR0 = 65535;

    //TB1CTL |= ID__4;          // setting divider D1
    TB0EX0 |= TBIDEX__7;        // setting divider D2

    TB0CCTL0 |= CCIE;           // Clear ISR flag
    TB0CCTL0 &= ~CCIFG;         // Enable timer interrupt

    __enable_interrupt();       // Enable Maskable IRQs
}

void ledarray_init(void) {
    // Configure LED pins as outputs
    P3DIR |= LED_PINS;
    P3OUT |= LED_PINS;  // All LEDs off initially
    
    init_timer();
}

void ledarray_all_off(void) {
    P3OUT |= LED_PINS;
    pattern_active = false;
    current_pattern = PATTERN_NONE;
}

void ledarray_select_pattern(led_pattern_t pattern) {
    if (pattern == current_pattern) {
        switch(current_pattern){
            case PATTERN_1_TOGGLE:
                pattern_step1 = 0;
                break;
            case PATTERN_2_UP_COUNT:
                pattern_step2 = 0;
                break;
            case PATTERN_3_IN_OUT:
                pattern_step3 = 0;
                break;
        }
    } else {
        current_pattern = pattern;
    }
    pattern_active = true;
}

void ledarray_set_transition_period(float period) {
    TB0CCTL0 |= CCIFG;         // Disable timer interrupt
    base_transition_period = period;
    
    // Update timer period
    unsigned int timer_counts = (unsigned int)(period * 65535); // Assuming 1MHz SMCLK
    TB0CCR0 = timer_counts;
    TB0CCTL0 |= CCIE;           // Clear ISR flag
    TB0CCTL0 &= ~CCIFG;         // Enable timer interrupt
}

float ledarray_get_transition_period(void) {
    return base_transition_period;
}

void ledarray_decrease_period(void) {
    if (base_transition_period > 0) {
        ledarray_set_transition_period(base_transition_period - 0.25);
    }
}

void ledarray_increase_period(void) {
    if(base_transition_period < 1){
        ledarray_set_transition_period(base_transition_period + 0.25);
    }
}

void ledarray_update(void) {
    if (!pattern_active) return;
    
    switch (current_pattern) {
        case PATTERN_0_STATIC:
            P3OUT = (P3OUT & ~LED_PINS) | STATIC_PATTERN;
            break;
            
        case PATTERN_1_TOGGLE:
            P3OUT = (P3OUT & ~LED_PINS) | TOGGLE_PATTERN[pattern_step1];
            pattern_step1 = (pattern_step1 + 1) % 2;
            break;
            
        case PATTERN_2_UP_COUNT:
            P3OUT = (P3OUT & ~LED_PINS) | pattern_step2;
            pattern_step2 = (pattern_step2 + 1) % 256;
            break;
            
        case PATTERN_3_IN_OUT:
            P3OUT = (P3OUT & ~LED_PINS) | IN_OUT_PATTERN[pattern_step3];
            pattern_step3 = (pattern_step3 + 1) % 6;
            break;
            
        default:
            P3OUT |= LED_PINS;
            break;
    }
}

// Timer A1 interrupt handler
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer1_A0_ISR(void) {
    // Update LED pattern
    ledarray_update();
}