#include <msp430fr2355.h>

// LCD Pin Definitions (adjust based on your hardware)
#define RS          BIT0    // P1.0 - Register Select
#define RW          BIT1    // P1.1 - Read/Write (optional, can be grounded)
#define EN          BIT2    // P1.2 - Enable
#define DATA_PORT   P1OUT   // Data pins on P2.4-P2.7 (DB4-DB7)
#define DATA_DIR    P1DIR
#define DATA_MASK   0xF0    // Upper 4 bits of P2 for data

// LCD Commands
#define LCD_CLEAR   0x01    // Clear display
#define LCD_HOME    0x02    // Return home
#define LCD_ENTRY   0x06    // Entry mode: increment, no shift
#define LCD_ON      0x0C    // Display on, cursor off
#define LCD_OFF     0x08    // Display off
#define LCD_FUNC    0x28    // 4-bit, 2 lines, 5x8 font
#define LCD_LINE1   0x80    // First line address
#define LCD_LINE2   0xC0    // Second line address

// Function prototypes
void lcd_init(void);
void lcd_write_cmd(unsigned char cmd);
void lcd_write_data(unsigned char data);
void lcd_delay(unsigned int ms);
void lcd_print(char *str);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    // Configure GPIO
    P1DIR |= RS | RW | EN;        // Set RS, RW, EN as outputs
    P1OUT &= ~(RS | RW | EN);     // Initialize low
    DATA_DIR |= DATA_MASK;        // Set data pins as outputs
    DATA_PORT &= ~DATA_MASK;      // Clear data pins

    // Clock configuration (1 MHz default)
    __delay_cycles(50000);        // Wait for LCD power-up (>15ms @ 1MHz)

    lcd_init();                   // Initialize LCD


    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    // Display "Hello World"
    lcd_write_cmd(LCD_LINE1);     // Set cursor to line 1
    lcd_print("Hello");
    lcd_write_cmd(LCD_LINE2);     // Set cursor to line 2
    lcd_print("World");

    while(1) {
        // Main loop
    }
}

void lcd_init(void)
{
    lcd_delay(15);                // Wait >15ms after power-on

    // 4-bit mode initialization sequence (per KS0066U/HD44780 spec)
    P1OUT &= ~(RS | RW);          // RS=0 (command), RW=0 (write)

    // First 0x03
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (0x03 << 4);     // Send 0x30 high nibble
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;
    lcd_delay(5);                 // Wait >4.1ms

    // Second 0x03
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (0x03 << 4);     // Send 0x30 high nibble
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;
    lcd_delay(5);                 // Wait >100us

    // Third 0x03
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (0x03 << 4);     // Send 0x30 high nibble
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;
    lcd_delay(5);                 // Wait >100us

    // Set 4-bit mode (0x02)
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (0x20 << 4);     // Send 0x20 high nibble
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;
    lcd_delay(1);

    // Now in 4-bit mode, send full commands
    lcd_write_cmd(LCD_FUNC);      // 4-bit, 2 lines, 5x8 font
    lcd_write_cmd(LCD_OFF);       // Display off
    lcd_write_cmd(LCD_CLEAR);     // Clear display
    lcd_write_cmd(LCD_ENTRY);     // Entry mode: increment, no shift
    lcd_write_cmd(LCD_ON);        // Display on, cursor off
}

void lcd_write_cmd(unsigned char cmd)
{
    P1OUT &= ~(RS | RW);          // RS=0 (command), RW=0 (write)

    // High nibble
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (cmd & 0xF0);    // Send upper 4 bits
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;

    // Low nibble
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= ((cmd << 4) & 0xF0); // Send lower 4 bits
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;

    lcd_delay(2);                 // Wait for command to complete (>37us)
}

void lcd_write_data(unsigned char data)
{
    P1OUT |= RS;                 // RS=1 (data)
    P1OUT &= ~RW;                // RW=0 (write)

    // High nibble
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= (data & 0xF0);   // Send upper 4 bits
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;

    // Low nibble
    DATA_PORT &= ~DATA_MASK;
    DATA_PORT |= ((data << 4) & 0xF0); // Send lower 4 bits
    P1OUT |= EN;
    __delay_cycles(1);
    P1OUT &= ~EN;

    lcd_delay(1);                 // Wait for data write (>37us)
}

void lcd_print(char *str)
{
    while(*str) {
        lcd_write_data(*str++);
    }
}

void lcd_delay(unsigned int ms)
{
    while(ms--) {
        __delay_cycles(1000);     // 1ms delay @ 1MHz clock
    }
}