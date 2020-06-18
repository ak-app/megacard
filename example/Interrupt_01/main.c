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
 *   to increment LEDs with ISR
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

// ISR (Interrupt Service Routine) for INT1
// Immediately called after an interrupt request occurred
ISR(INT1_vect)
{
    PORTC++;    // Increment PORT
    
    // Because the switch is bouncing the ISR gets called several times
    // so it seems that PORTC is incrementing randomly
}

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
    GICR = (1<<INT1);       // Enable ISR(INT1_vect) for external interrupt 1
    
    sei();                  // Enable interrupts globally (set I-Bit)
    
    while (1) 
    {
    }
}