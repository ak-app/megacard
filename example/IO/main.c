/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Switching leds on/off with an
 *   adjustable delay
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <avr/io.h>         // Required for I/O
#include <util/delay.h>     // Required for _delay_ms()

int main(void)
{
    // Data direction register of PORT as output
    DDRC = 0xFF;    // other options:   PORTC = 0b11111111;
                    //                  PORTC = (1<<PB7) | (1<<PB6) | (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
    
    // Switch PORT level to GROUND
    PORTC = 0x00;
    
    
    // Data direction register of PORT as input
    DDRA = 0x00;
    
    // Enable Pullup resistors on PORT(3:0)
    PORTA = 0x0F;   // other options:   PORTA = 0b00001111;
                    //                  PORTA = (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);

    while (1) 
    {
        PORTC = ~PORTC;     // Read actual PORT values an invert
        
        // Check PINA0 (HIGH -> LOW)
        if(!(PINA & (1<<PA0)))          // other options:   if(!(PINA & (1<<0)))
        {                               //                  if(!(PINA & 0x01))
            _delay_ms(1000);            // Wait for 1000ms
        }
        // Check PINA1 (HIGH -> LOW)
        else if(!(PINA & (1<<PA1)))     // other options:   if(!(PINA & (1<<1)))
        {                               //                  if(!(PINA & 0x02))
            _delay_ms(750);             // Wait for 750ms
        }
        // Check PINA2 (HIGH -> LOW)
        else if(!(PINA & (1<<PA2)))     // other options:   if(!(PINA & (1<<2)))
        {                               //                  if(!(PINA & 0x04))
            _delay_ms(500);             // Wait for 500ms
        }
        // Check PINA3 (HIGH -> LOW)
        else if(!(PINA & (1<<PA3)))     // other options:   if(!(PINA & (1<<4)))
        {                               //                  if(!(PINA & 0x08))
            _delay_ms(250);             // Wait for 250ms
        }
        else
        {
            _delay_ms(2500);            // Wait for 250ms
        }        
    }
}
