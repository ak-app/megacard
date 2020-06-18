/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Creating sound with adjustable
 *   frequencies ans duty cycle. Controlled
 *   with Timer0
 * -----------------------------------------
 *          Related data sheets
 *
 * - ATmega16.pdf
 * - ATmega16 - Timer0 Register.pdf
 * -----------------------------------------
 */


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Hardware setup
//
//             +-----+
//             |  U  |
// +-----------+  S  +--------------+
// | +----+    |  B  |  +---------+ |
// | | X6 |    +-----+  | DISPLAY | +---+
// | +----+             +---------+ | P |
// | CONNECT X6!                    | O |
// |                                | R |
// |     M                          | T |
// |     E                          | A | -> PA4 -----------+
// |     G                          | / |                   |
// |     A                          | C |                   |
// |     C                          |   |                   |
// |     A                   +---+  +---+                   |
// |     R                   | P |  |                       |
// |     D                   +---+  +---+                   |
// |                                | P |                   |
// |                                | O |                   |
// |                                | R |                   |
// |                                | T | -> PB3 (OC0) -----+
// |                                | B |
// |                                | / |
// |                                | D |
// |                                |   |
// |                                +---+
// |                                |
// +--------------------------------+
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#define F_CPU 12000000UL    // System clock (required for delay)

// Preprocessor instructions
#define HIGH 0xFF   // 0b1111_1111
#define LOW 0x00    // 0b0000_0000

// System libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void)
{
    // Data direction register of PORT(3) as output
    DDRB |= (1<<PC3);   // other options:   PORTC |= 0b00000001;
 
 
    // Data direction register of PORT as output
    DDRC = HIGH;    // other options:   PORTC = 0b11111111;
    
    // Switch PORT level to GROUND
    PORTC = LOW;    // other options:   PORTC = 0b00000000;
       
    
    // Data direction register of PORT(3:0) as input and leave PORT(7:4) untouched
    DDRA &= ~(0x0F);
        
    // Enable pullup resistors on PORT(3:0) and leave PORT(7:4) untouched
    PORTA |= 0x0F;   // other options:   PORTA |= 0b00001111;
    
    
    // Timer0 settings
    // -      Mode: Mode 1 (PWM phase correct)
    // - Operation: Clear OC0 on compare match when upcounting / Set OC0 on compare match when down counting
    // - Prescaler: f_CPU/variable
    // -  Handling: Automatic with counter Hardware
    TCCR0 = (1<<WGM00) | (1<<COM01);        // Setup Mode 1 for Timer0 with OC0 (PB3) enabled
    OCR0 = 127;                             // Setup duty cycle to 50% (startup value)
    
    
    // In mode 1 (PWM phase correct) counter is up/down counting from 0 to 255.
    // With the OCR register the duty cycle of the signal can be adjusted. The
    // prescaler (1/8/64/256 or 1024) allows several different frequencies. With
    // special counter operations OC0 can be set from the counter itself. Only the
    // pin has to be defined as output
    //
    // Calculation:
    //
    //
    //           f_CPU
    // f_PWM = ---------            N... Prescaler (1/8/64/256 or 1024)
    //          N * 510
    //
    //            1
    // T_PWM = -------
    //          f_PWM
    //
    // --------------------------------------------
    // f_PWM(N=1)    = 23529.41 Hz    ->   42.50 us
    // f_PWM(N=8)    =  2941.42 Hz    ->  340    us
    // f_PWM(N=64)   =   367.65 Hz    ->    2.72 ms
    // f_PWM(N=256)  =    91.91 Hz    ->   10.88 ms
    // f_PWM(N=1024) =    22.98 Hz    ->   43.52 ms
    // --------------------------------------------
    //
    // Calculation of the duty cycle (OCR0 = 0 - 255)
    //
    //           2 * OCR0            2 * 127
    // ?_PWM = ----------- * 100% = --------- * 100 = 49.80% ~ 50% 
    //              510               510
    //
    // Calculation of OCR0 (?_PWM = 0 - 100%)
    //
    //         ?_PWM * 510     25% * 510
    // OCR0 = ------------- = ----------- = 63.75 ~ 64
    //           2 * 100%      2 * 100%
    //
    
    PORTC = OCR0;   // Write OCR0 value to LEDs
    
    
    while (1) 
    {
        // +----------------------------+
        // | Setup frequency            |
        // +----------------------------+
        
        // Multiple branch
        // Check if S0 (PINA0) is pressed (pushed -> LOW)
        // ~PINA -> ~ 0bXXXXXX??
        //          & 0b0000XX11
        //          ------------
        //          = 0b000000??
        // if   true    when ?? == 0x01
        //      false   when ?? != 0x01
        if((~(PINA) & 0x03) == 0x01)
        {
            // Setup prescaler to /64
            TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
            TCCR0 |= (1<<CS01) | (1<<CS00);
        }
        // Multiple branch
        // Check if S1 (PINA1) is pressed (pushed -> LOW)
        // ~PINA -> ~ 0bXXXXXX??
        //          & 0b0000XX11
        //          ------------
        //          = 0b000000??
        // if   true    when ?? == 0x02
        //      false   when ?? != 0x02
        else if((~(PINA) & 0x03) == 0x02)
        {
            // Setup prescaler to /256
            TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
            TCCR0 |= (1<<CS02);
        }
        // Multiple branch
        // Check if S1 (PINA1) and S0 (PINA0) are pressed (pushed -> LOW)
        // ~PINA -> ~ 0bXXXXXX??
        //          & 0b0000XX11
        //          ------------
        //          = 0b000000??
        // if   true    when ?? == 0x03
        //      false   when ?? != 0x03
        else if((~(PINA) & 0x03) == 0x03)
        {
            // Setup prescaler to /1024
            TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
            TCCR0 |= (1<<CS02) | (1<<CS00);   
        }
        // If condition is not obtained, this program chapter will be executed
        else
        {
             // Setup prescaler to /8
             TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
             TCCR0 |= (1<<CS01);
        }
        
        
        // +----------------------------+
        // | Change duty cycle          |
        // +----------------------------+
        
        // Multiple branch
        // Check if S2 (PINA2) is pressed (pushed -> LOW)
        // ~PINA -> ~ 0bXXXX??XX
        //          & 0b000011XX
        //          ------------
        //          = 0b0000??00
        // if   true    when ?? == 0x04
        //      false   when ?? != 0x04
        if((~(PINA) & 0x0C) == 0x04)
        {
            OCR0++;         // Increment OCR0 register
            PORTC = OCR0;   // Write OCR0 value to LEDs
            
            _delay_ms(100);
        }
        // Multiple branch
        // Check if S3 (PINA3) is pressed (pushed -> LOW)
        // ~PINA -> ~ 0bXXXX??XX
        //          & 0b00001100
        //          ------------
        //          = 0b0000??00
        // if   true    when ?? == 0x08
        //      false   when ?? != 0x08
        else if((~(PINA) & 0x0C) == 0x08)
        {
            OCR0--;         // Decrement OCR0 register
            PORTC = OCR0;   // Write OCR0 value to LEDs
            
            _delay_ms(100);
        }
    }
}