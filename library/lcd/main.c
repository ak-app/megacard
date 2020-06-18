/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Sample for writing data to display with
 *   lcd library
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

// User libraries
#include "lcd.h"

int main(void)
{
    // Port setup
    DDRA &= ~(0x0F);    // Set PORT(3:0) as input
    PORTA = 0x0F;       // Enable pullup resistor
    
    // Defined pattern for LCD CGRAM 64 byte / 8 = 8 Characters
    // Size of LCD display character 5 * 8 bit
    // Byte 0:  *****
    // Byte 1:  *****
    // Byte 2:  *****
    // Byte 3:  *****
    // Byte 4:  *****
    // Byte 5:  *****
    // Byte 6:  *****
    // Byte 7:  *****
    
    // Bell character
    unsigned char pattern_bell[] =  {
                                    0b00100,
                                    0b01110,
                                    0b01110,
                                    0b01110,
                                    0b11111,
                                    0b00000,
                                    0b00100,
                                    0b00000
                                    };

    unsigned char pattern_L[] = { 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11111, 0b11111 }; // special L character
    unsigned char pattern_I[] = { 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110, 0b01110 }; // special I character
    unsigned char pattern_E[] = { 0b11111, 0b11111, 0b11000, 0b11111, 0b11111, 0b11000, 0b11111, 0b11111 }; // special E character
    unsigned char pattern_B[] = { 0b11110, 0b11111, 0b11011, 0b11111, 0b11111, 0b11001, 0b11111, 0b11110 }; // special B character
    unsigned char pattern_H[] = { 0b11011, 0b11011, 0b11011, 0b11111, 0b11111, 0b11011, 0b11011, 0b11011 }; // special H character
    unsigned char pattern_R[] = { 0b11110, 0b11111, 0b11011, 0b11011, 0b11111, 0b11110, 0b11110, 0b11011 }; // special R character
    
    // LCD display initialisation
    lcd_init();
    
    // Save self defined patterns to CGRAM
    lcd_pattern(LCD_CGADDR_CHAR0, pattern_bell);
    lcd_pattern(LCD_CGADDR_CHAR1, pattern_L);
    lcd_pattern(LCD_CGADDR_CHAR2, pattern_I);
    lcd_pattern(LCD_CGADDR_CHAR3, pattern_E);
    lcd_pattern(LCD_CGADDR_CHAR4, pattern_B);
    lcd_pattern(LCD_CGADDR_CHAR5, pattern_H);
    lcd_pattern(LCD_CGADDR_CHAR6, pattern_R);
    
    // Write data to first Line
    lcd_home();                             // Set cursor to home position
    lcd_char('N');                          // Write single character to LCD display
    lcd_string("UM");                       // Write string to LCD display
    lcd_ul2ascii(12, 10, 4);                // Write unsigned long to LCD display (4 fields reserved)
    //lcd_sl2ascii(-12, 10, 0);             // Write signed long to LCD display (no field reserved)
    //lcd_d2ascii(2.22, 4, 2);              // Write double to LCD display 
    lcd_char(LCD_CGADDR_CHAR0);             // Write self defined pattern to LCD display
    
    // Write data to second Line
    lcd_cursor(0, 1);                       // Set cursor to second line
    
    // Write defined patterns to LCD display
    lcd_char(LCD_CGADDR_CHAR1);
    lcd_char(LCD_CGADDR_CHAR2);
    lcd_char(LCD_CGADDR_CHAR3);
    lcd_char(LCD_CGADDR_CHAR4);
    lcd_char(LCD_CGADDR_CHAR5);
    lcd_char(LCD_CGADDR_CHAR3);
    lcd_char(LCD_CGADDR_CHAR6);
    lcd_char(LCD_CGADDR_CHAR6);
    
    // Endless loop
    while (1) 
    {
        
        // Check if PA0 (S0) is pressed
        if(!(PINA & (1<<PA0)))
        {
            lcd_shift(0);               // Shift text on lcd display right
            _delay_ms(5);               // Delay for debouncing button
        } while(!(PINA & (1<<PA0)));    // Wait until button is released
        _delay_ms(5);                   // Delay for debouncing button
        
        // Check if PA1 (S1) is pressed
        if(!(PINA & (1<<PA1)))
        {
            // Run through all columns in line
            for (unsigned char i=0; i < LCD_COLUMNS_END; i++)
            {
                // Check if text appears on the screen
                if((i < LCD_COLUMNS) || (i >= (LCD_COLUMNS_END - LCD_COLUMNS)))
                {
                    lcd_shift(0);       // Shift text on lcd display right
                    _delay_ms(500);     // Decrease speed of shifting
                }
                else
                {
                    lcd_shift(0);       // Shift text on lcd display right
                }
            }
        }
    }
}