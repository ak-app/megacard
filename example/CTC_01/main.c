/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Creating a signal that every second
 *   appears to generate a clock with ISR
 * -----------------------------------------
 *          Related data sheets
 *
 * - ATmega16.pdf
 * - ATmega16 - Timer0 Register.pdf
 * -----------------------------------------
 */


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Software setup
//
//         7             :             0
//       +-------------------------------+
// PORTC | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
//       +-------------------------------+
//        _______________________     ___
//              Minutes             Seconds
//              0 - 59             (Impulse)
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define F_CPU 12000000UL    // System clock (required for calculation)

// Preprocessor instructions
#define HIGH 0xFF       // 0b1111_1111
#define LOW 0x00        // 0b0000_0000

#define VALUE 10000UL   // Top of software counter 

// System libraries
#include <avr/io.h>
#include <avr/interrupt.h>

// Global variables
volatile unsigned int counter;  // Variables that are changed in an ISR have to be volatile!
                                // Otherwise data inconsistence is possible!
                                // Global variables are initialized with 0

volatile unsigned char seconds;
volatile unsigned char minutes;
volatile unsigned char hours;

// ISR (Interrupt Service Routine) for Timer0 compare match
// Immediately called after an interrupt request occurred
ISR(TIMER0_COMP_vect)
{
    counter++;              // Increment software counter
    
    // Check if counter exceeds VALUE
    if (counter >= VALUE)
    {
        // Time calculation
        // Seconds: 0-59
        // Minutes: 0-59
        //   Hours: 0-24
        
        seconds++;
        
        if(seconds > 59)
        {
            minutes++;
            
            if(minutes > 59)
            {
                hours++;
                
                if(hours > 23)
                {
                    hours = 0;
                }
                minutes = 0;
            }
            seconds = 0;
        }
        // Reset counter
        counter = 0;
    }
}

int main(void)
{
    // Data direction register of PORT as output
    DDRC = HIGH;            // other options:   PORTC = 0b11111111;
    
    // Switch PORT level to GROUND
    PORTC = LOW;            // other options:   PORTC = 0b00000000;
    
    
    // Data direction register of PORT(3:0) as input and leave PORT(7:4) untouched
    DDRA &= ~(0x0F);
    
    // Enable pullup resistors on PORT(3:0) and leave PORT(7:4) untouched
    PORTA |= 0x0F;   // other options:   PORTA |= 0b00001111;
    
    
    // Timer0 settings
    // -      Mode: Mode 2 (CTC)
    // - Prescaler: f_CPU/8
    // -  Handling: Interrupt
    TCCR0 = (1<<WGM01) | (1<<CS01); // Setup prescaler for Timer0 and CTC, all other options are already set
    TIMSK |= (1<<OCIE0);            // Enable ISR(TIMER0_COMP_vect) for Timer0
    
    sei();                          // Enable interrupts globally (set I-Bit)
    
    // In mode 2 (CTC) counter is counting from 0 to value of OCR and than gets cleared.
    // If a compare match occurs an interrupt is generated.
    //
    // Calculation:
    //
    //               f_CPU
    // f_INT = ----------------         N... Prescaler (1/8/64/256 or 1024)
    //          N * (1 + OCR0)
    //
    // Generation of an interrupt every second
    //
    //            1        1
    // f_INT = ------- = ---- = 1 Hz 
    //          T_INT     1s
    //
    //          f_CPU            12 * 10^6 Hz
    // OCR0 = ----------- - 1 = -------------- = 1.5 * 10^6 -> not possible
    //         N * f_INT           8 * 1 Hz
    //
    // OCR0 can only take values from 1 - 255 because it is an 8 bit register
    //
    //  OCR0        |    Time
    // -------------+------------
    // 1.5 * 10^6   |     1s
    // 150          |     t
    //
    //               150
    // t = 1s * ------------ = 100 * 10^-6 s = 100 us 
    //           150 * 10^4
    //
    // With OCR=150 it is possible to generate an compare match interrupt
    // every 100 us. Now it is necessary to calculate back to 1 second to
    // get the VALUE of the software counter which will be needed
    //
    //          1s          1
    // VALUE = ---- = ------------- = 10000
    //          t      100 * 10^-6
    //
    // So every 10000 cycles the software counter should fire (1s reached)
    
    OCR0 = 150;     // Setup calculated OCR0 value;
    
    while (1)
    {
        // Write time to PORT(7:2 & 0)
        PORTC = (0xFC & (minutes<<2)) | (0x01 & ((seconds%2)<<0));
    }
}