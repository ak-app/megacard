/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Waiting for a falling edge on INT1 
 *   to increment LEDs with polling
 * -----------------------------------------
 *          Related data sheets
 *
 * - ATmega16.pdf
 * - ATmega16 - INTx Register.pdf
 * - Megacard - Schematic.pdf
 * - Megacard - Interfaces.pdf
 * -----------------------------------------
 */


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Hardware setup
//
//             +-----+
//             |  U  |
// +-----------+  S  +--------------+
// |     M     |  B  |  +---------+ |
// |     E     +-----+  | DISPLAY | +---+
// |     G              +---------+ | P | -> PA0 (S0) ------+
// |     A                          | O |                   | W
// |     C                          | R |                   | I
// |     A                          | T |                   | R
// |     R                          | A |                   | E
// |     D                          | / |                   |
// |                                | C |                   |
// |                                |   |                   |
// |                         +---+  +---+                   |
// |                         | P |  |                       |
// |                         +---+  +---+                   |
// |                                | P |                   |
// |                                | O |                   |
// |                                | R |                   |
// |                                | T |                   |
// |                                | B | -> PD3 (INT1) ----+
// |                                | / |
// |                                | D |
// |                                |   |
// |                                +---+
// |                                |
// +--------------------------------+
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Preprocessor instructions
#define HIGH 0xFF   // 0b1111_1111
#define LOW 0x00    // 0b0000_0000

// System libraries
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
    // Data direction register of PORT as output
    DDRC = HIGH;            // other options:   PORTC = 0b11111111;
    
    // Switch PORT level to GROUND
    PORTC = LOW;            // other options:   PORTC = 0b00000000;
    
    
    // Data direction register of PORT(3) as input and leave PORT(7:4 & 2:0) untouched
    DDRD &= ~(1<<PD3);
    
    // Enable pullup resistors on PORT(3) and leave PORT(7:4 & 2:0) untouched
    PORTD |= (1<<PD3);   // other options:   PORTA |= 0b00001000;
    
    
    // External interrupt 1 settings
    MCUCR = (1<<ISC11);     // Setup interrupt on falling edge at PD2 (INT1)
    
    // Globally interrupt enabling is not necessary because ISR will not be
    // executed in polling mode
    
    
    while (1) 
    {
        // Single branch
        // Check if INTF1 is set by hardware
        // GIFR -> 0b?XXXXXXX
        //       & 0b10000000
        //       ------------
        //       = 0b?0000000
        // if   true    when ? == 1
        //      false   when ? == 0
        if(GIFR & (1<<INTF1))
        {
            PORTC++;                // Increment PORT
            GIFR |= (1<<INTF1);     // Reset interrupt flag
            
            // Because the switch is bouncing the ISR gets called several times
            // so it seems that PORTC is incrementing randomly
            
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // !!! For resetting interrupt flags it is required to write a 1    !!!
            // !!! to the position where the interrupt flag is placed in the    !!!
            // !!! particular register.                                         !!!
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
        
    }
}