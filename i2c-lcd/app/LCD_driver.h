#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// LCD Command Definitions for KS0066 Controller (4-bit mode)
#define LCD_CLEAR_DISPLAY    0x01  // Clear display, return cursor to home
#define LCD_RETURN_HOME      0x02  // Return cursor to home position
#define LCD_ENTRY_MODE_SET   0x06  // Cursor increment, no display shift
#define LCD_DISPLAY_ON       0x0C  // Display ON, Cursor OFF, Blink OFF
#define LCD_CURSOR_ON        0x0E  // Display ON, Cursor ON, Blink OFF
#define LCD_CURSOR_BLINK     0x0F  // Display ON, Cursor ON, Blink ON
#define LCD_CURSOR_LEFT      0x10  // Shift cursor left
#define LCD_CURSOR_RIGHT     0x14  // Shift cursor right
#define LCD_FUNCTION         0x28  // 4-bit mode, 2 lines, 5x8 font
#define LCD_BOTTOM_LINE      0xC0  // DDRAM address for start of second line

// Function Prototypes
void init_lcd(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_send_string(char *str);
void lcd_set_key_char(uint8_t data);
void lcd_toggle_cursor(void);
void lcd_toggle_blink(void);
void lcd_set_function(void);
void lcd_clear_top(void);
void lcd_clear_bottom(void);

#endif /* LCD_H */