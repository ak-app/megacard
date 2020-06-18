/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Switching leds on/off with user input
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// Preprocessor instructions
#define HIGH 0xFF   // 0b1111_1111
#define LOW 0x00    // 0b0000_0000

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
    
    
    while (1) 
    {
        // Case distinction
        // ~PINA -> ~ 0bXXXXXX??
        //          & 0b00000011
        //          ------------
        //          = 0b000000??
        switch ((0x03 & ~(PINA)))
        {
            // Check if S0 (PINA0) is pressed (pushed -> LOW)
            //      true    when ?? == 0x01
            //      false   when ?? != 0x01
            case 0x01   :   PORTB = 0xF0;   break;  // Switch PORT(7:4) level to VCC (turn on LEDs)
            
            // Check if S1 (PINA1) is pressed (pushed -> LOW)
            //      true    when ?? == 0x02
            //      false   when ?? != 0x02
            case 0x02   :   PORTB = 0x0F;   break;  // Switch PORT(3:0) level to VCC (turn on LEDs)
            
            // If conditions are not obtained, default chapter will be executed
            default     :   PORTB = LOW;    break;  // Switch PORT level to GROUND (switch off LEDs)
        }
    }
}