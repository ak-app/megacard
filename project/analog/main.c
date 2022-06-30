/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 2.0 Release
 *      Author: G.Raf
 * Description:
 *   Read data from A/D channel 5 and send
 *   data calculated to mV over UART
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
// |     G              +---------+ | P |
// |     A                          | O |
// |     C                          | R |
// |     A                          | T |
// |     R                          | A |
// |     D                          | / |
// |                                | C |
// |                                |   |                                                 +-----------------------------+
// |                         +---+  +---+                                                 |                             |
// |                         | P |  |                       +-----------+                 | +-----------------------+   |
// |                         +---+  +---+                   |   U   +---+---+             | | TeraTerm              |   |
// |                                | P |                   |   S   |  USB  |-------------+ | ~~~~~~~~              |   |
// |                                | O |                   |   B   +---+---+             | | ~~~~~                 |   |
// |                                | R |                   |   /       |                 | | ~~~~~~~~~~~~          |   |
// |                                | T |                   |   U       |                 | |                       |   |
// |                                | B |                   |   A       |                 | +-----------------------+   |
// |                                | / |                   |   R       |                 |                             |
// |                                | D | -> PD1 -- TXD <-  |   T       |                 | Computer                    |
// |                                |   | -> PD0 -- RXD <-  |  +--------+                 |                             |
// +                                +---+ -> GND -- GND <-  |  | 5V/3V3 |                 |                             |
// |                                |                       +--+--------+                 |                             |
// |     +-----+     +--------+     |                                                     |                             |
// |     | ISP |     |  JTAG  |     |                                                     +-----------------------------+
// +-----+-----+-----+--------+-----+
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// TeraTerm (COM?|8E1@2400)
//
// ----------------------CONSOLE I/O----------------------
// Voltage: ???? mV
// -------------------------------------------------------
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <avr/io.h>         // Required for I/O
#include <util/delay.h>     // Required for _delay_ms()

// User libraries
#include "../../library/adc/adc.h"
#include "../../library/uart/uart.h"

int main(void)
{
    // Port setup
    DDRC = 0xFF;    // Set PORT as output
    
    // Initialize UART(8N1) with defined parameters in uart.h
    uart_init();
    
    // Initialize ADC with defined parameters in adc.h
    adc_init();
    
    // Select ADC channel 5 (unipolar mode)
    adc_channel(ADC_CH5);
    
    while (1) 
    {
        // Read data from A/D channel 5 and
        // adjust it right
        unsigned int data = (adc_read()>>6);
        
        // Write converted value to PORT
        // PORT only has 8 Bit, so 2 bits
        // have to be thrown away!
        PORTC = (unsigned char)(data>>2);
        
        // Calculation the Voltage in mV on ADC channel(5)
        //
        //                5000 mV
        // U_mV = data * --------- 
        //                 2^10
        //
        // Better (faster) option for dividing /1024 is shifting 10 bits right (2^10)
        // unsigned int mv = (data * 1000UL) / 1024UL;
        unsigned int mv = ((data * 5000UL)>>10);
        
        // Send Voltage status over UART
        printf("Voltage: %4u mV\r", mv);
        
        // Slows down the reloading of Console
        _delay_ms(100);
    }
}