/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Creating a delay of 500 ms with Timer0
 *   and turn LEDs on/off with ISR
 * -----------------------------------------
 *          Related data sheets
 *
 * - ATmega16.pdf
 * - ATmega16 - Timer0 Register.pdf
 * -----------------------------------------
 */

#define F_CPU 12000000UL    // System clock (required for calculation)

// Preprocessor instructions
#define HIGH 0xFF   // 0b1111_1111
#define LOW 0x00    // 0b0000_0000

// Calculate blink frequency of LEDs
#define VALUE (0.5 * F_CPU/(256UL * 8))

// System libraries
#include <avr/io.h>
#include <avr/interrupt.h>

// Global variables
volatile unsigned int counter;  // Variables that are changed in an ISR have to be volatile!
                                // Otherwise data inconsistence is possible!
                                // Global variables are initialized with 0

// ISR (Interrupt Service Routine) for Timer0 overflow
// Immediately called after an interrupt request occurred
ISR(TIMER0_OVF_vect)
{
    counter++;              // Increment software counter
    
    // Check if counter exceeds VALUE
    if (counter >= VALUE)
    {
        PORTC = ~PORTC;     // Invert PORT status
        counter = 0;        // Reset counter
    }
}

int main(void)
{
    // Data direction register of PORT as output
    DDRC = HIGH;            // other options:   PORTC = 0b11111111;
    
    // Switch PORT level to GROUND
    PORTC = LOW;            // other options:   PORTC = 0b00000000;
    
    
    // Timer0 settings
    // -      Mode: Mode 0 (Normal)
    // - Prescaler: f_CPU/8
    // -  Handling: Interrupt
    TCCR0 = (1<<CS01);      // Setup prescaler for Timer0, all other options are already set
    TIMSK |= (1<<TOIE0);    // Enable ISR(TIMER0_TOV_vect) for Timer0
    
    sei();                  // Enable interrupts globally (set I-Bit)
    
    // In mode 0 (normal) counter is counting from 0 to 255.
    // If an overflow occurs an interrupt is generated.
    //
    // Calculation:
    //
    //           f_CPU      12*10^6 Hz
    // f_OVF = --------- = ------------ = 5859.37 Hz ~ 5859 Hz          N... Prescaler (1/8/64/256 or 1024)
    //          N * 2^8      8 * 256
    //
    //            1            1            _
    // T_OVF = ------- = ------------ = 170.6 us
    //          f_OVF     5859.37 Hz
    //
    // An Timer0 overflow interrupt is generated every 170us.
    // To extend the time it is necessary to implement a software counter.
    // In this example the software counter should exceed every 500 ms
    //
    //                  500 * 10^-3
    // EXCEED VALUE = ---------------- = 2929.69 ~ 2930
    //                 170.66 * 10^-6
    //
    // So every 2930 cycles the software counter should fire
    
    
    while (1) 
    {
    }
}