/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Check how often a switch is bouncing
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// Preprocessor instructions
#define HIGH 0xFF   // 0b1111_1111
#define LOW 0x00    // 0b0000_0000

#define SWITCH PINA0  // Switch S0 (PA0)

// System libraries
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // Data direction register of PORT as output
    DDRC = HIGH;    // other options:   PORTC = 0b11111111;
    
    // Switch PORT level to GROUND
    PORTC = LOW;    // other options:   PORTC = 0b00000000;
    
    
    // Data direction register of PORT(3:0) as input and leave PORT(7:4) untouched
    DDRA &= ~(0x0F);
    
    // Enable pullup resistors on PORT(3:0) and leave PORT(7:4) untouched
    PORTA |= 0x0F;   // other options:   PORTA |= 0b00001111;
    
    // Initialize PIN(0) to VCC
    PORTC = 0x01;
    
    while (1)
    {
        // Single branch
        // Check if S0 (PINA0) is pressed (pushed -> LOW)
        // PINA -> 0bXXXXXXX?
        //       & 0b00000001
        //       ------------
        //       = 0b0000000?
        // if !(true)   when ? == 1
        //    !(false)  when ? == 0
        if(!(PINA & (1<<SWITCH)))
        {
            PORTC = (PORTC<<1) | (PORTC>>7);    // Rotate PORT one position (left)
            
            while(!(PINA & (1<<SWITCH)));       // Wait until S0 (PINA0) is released (pushed -> HIGH)
        }
    }
}