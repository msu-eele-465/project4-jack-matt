/**
 * LCD Display Test for MSP430FR2310
 *
 * This program interfaces with a KS0066U-based LCD display (MTC-S16204XFGHSAY-01)
 * using 4-bit interface mode to display "Hello World".
 *
 * LCD Connection (4-bit mode):
 * - RS (Register Select) -> P1.0
 * - R/W (Read/Write)     -> P1.1 (tied to GND if write-only operation)
 * - E (Enable)           -> P1.2
 * - DB4                  -> P1.4
 * - DB5                  -> P1.5
 * - DB6                  -> P1.6
 * - DB7                  -> P1.7
 *
 * Note: Connect VSS to GND, VDD to +5V or +3.3V based on your LCD spec,
 * and adjust contrast using V0 pin with a potentiometer.
 */

#include <msp430.h>
#include <stdint.h>

// Pin definitions for LCD control
#define LCD_RS_PIN BIT0 // P1.0 - Register Select
#define LCD_RW_PIN BIT1 // P1.1 - Read/Write
#define LCD_E_PIN BIT2 // P1.2 - Enable
#define LCD_DATA_PORT P1OUT
#define LCD_DATA_DIR P1DIR

// LCD instructions
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRY_MODE_SET 0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_CURSOR_SHIFT 0x10
#define LCD_FUNCTION_SET 0x20
#define LCD_SET_CGRAM_ADDR 0x40
#define LCD_SET_DDRAM_ADDR 0x80

// LCD entry mode bits
#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_LEFT 0x02
#define LCD_ENTRY_SHIFT_INCREMENT 0x01
#define LCD_ENTRY_SHIFT_DECREMENT 0x00

// LCD control bits
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00

// LCD function set bits
#define LCD_8BIT_MODE 0x10
#define LCD_4BIT_MODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Function prototypes
void clockInit(void);
void delayMs(uint16_t ms);
void delayUs(uint16_t us);
void lcdInit(void);
void lcdSendCommand(uint8_t command);
void lcdSendData(uint8_t data);
void lcdSendNibble(uint8_t nibble);
void lcdString(const char *str);
void lcdSetCursor(uint8_t row, uint8_t col);

/**
 * Main function
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Initialize clock
    clockInit();

    // Configure LCD pins as outputs
    LCD_DATA_DIR |= (LCD_RS_PIN | LCD_RW_PIN | LCD_E_PIN | 0xF0); // Upper 4 bits for data

    // Initialize LCD
    lcdInit();

    // Display "Hello World" on the LCD
    lcdSendCommand(LCD_CLEAR_DISPLAY);
    delayMs(2); // Clear display command needs >1.64ms

    lcdString("Hello World");

    // Main loop
    while (1)
    {
        // Nothing to do here once text is displayed
    }

    return 0;
}

/**
 * Initialize the system clock to run at 8MHz
 */
void clockInit(void)
{
    // Configure DCO to 8MHz
    CSCTL0_H = CSKEY_H; // Unlock CS registers
    CSCTL1 = DCOFSEL_3; // DCO = 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // ACLK = VLOCLK, SMCLK = MCLK = DCO
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Set all dividers to 1
    CSCTL0_H = 0; // Lock CS registers
}

/**
 * Delay function in milliseconds
 */
void delayMs(uint16_t ms)
{
    // This delay is approximate - based on an 8MHz clock
    while (ms--)
    {
        __delay_cycles(8000); // 8000 cycles at 8MHz = ~1ms
    }
}

/**
 * Delay function in microseconds
 */
void delayUs(uint16_t us)
{
    // This delay is approximate - based on an 8MHz clock
    while (us--)
    {
        __delay_cycles(8); // 8 cycles at 8MHz = ~1us
    }
}

/**
 * Initialize the LCD
 */
void lcdInit(void)
{
    // Wait for LCD to power up and be ready
    delayMs(50);

    // Set RS and R/W low for command mode, no read
    LCD_DATA_PORT &= ~(LCD_RS_PIN | LCD_RW_PIN);

    // Special initialization sequence for 4-bit mode
    // First, force 8-bit mode (3 times)
    lcdSendNibble(0x03);
    delayMs(5);
    lcdSendNibble(0x03);
    delayMs(1);
    lcdSendNibble(0x03);
    delayMs(1);

    // Now set to 4-bit mode
    lcdSendNibble(0x02);
    delayMs(1);

    // Function set: 4-bit mode, 2 lines, 5x8 dots
    lcdSendCommand(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS);

    // Display control: Display on, cursor off, blink off
    lcdSendCommand(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);

    // Clear display
    lcdSendCommand(LCD_CLEAR_DISPLAY);
    delayMs(2); // Clear display command needs >1.64ms

    // Entry mode set: Increment cursor position, no display shift
    lcdSendCommand(LCD_ENTRY_MODE_SET | LCD_ENTRY_LEFT);
}

/**
 * Send a command to the LCD
 */
void lcdSendCommand(uint8_t command)
{
    // RS low for command
    LCD_DATA_PORT &= ~LCD_RS_PIN;

    // Send command in two 4-bit operations
    lcdSendNibble(command >> 4); // Send upper 4 bits
    lcdSendNibble(command & 0x0F); // Send lower 4 bits

    // Most commands require at least 37us delay
    delayUs(50);
}

/**
 * Send data to the LCD
 */
void lcdSendData(uint8_t data)
{
    // RS high for data
    LCD_DATA_PORT |= LCD_RS_PIN;

    // Send data in two 4-bit operations
    lcdSendNibble(data >> 4); // Send upper 4 bits
    lcdSendNibble(data & 0x0F); // Send lower 4 bits

    // Data writes require at least 37us delay
    delayUs(50);
}

/**
 * Send a 4-bit nibble to the LCD
 */
void lcdSendNibble(uint8_t nibble)
{
    // Put nibble value on upper 4 bits of port
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (nibble << 4);

    // Toggle enable pin to latch data
    LCD_DATA_PORT |= LCD_E_PIN;
    delayUs(1); // Enable pulse must be >450ns
    LCD_DATA_PORT &= ~LCD_E_PIN;
    delayUs(1); // Enable cycle time must be >1000ns
}

/**
 * Send a string to the LCD
 */
void lcdString(const char *str)
{
    while (*str)
    {
        lcdSendData(*str++);
    }
}

/**
 * Set the cursor position on the LCD
 */
void lcdSetCursor(uint8_t row, uint8_t col)
{
    static const uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

    // Set DDRAM address (row and column)
    lcdSendCommand(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}
