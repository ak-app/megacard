/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Switching leds on/off with a delay
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
    DDRC = HIGH;            // other options:   PORTC = 0xFF;
    
    
    // Switch PORT level to GROUND (switch off LEDs)
    PORTC = LOW;
    
    while (1) 
    {
        PORTC = ~PORTC;     // Invert PORT status
        _delay_ms(1000);    // Wait for 1000ms
    }
}