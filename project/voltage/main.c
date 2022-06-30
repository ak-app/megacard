/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 2.0 Release
 *      Author: G.Raf
 * Description:
 *   Read data from A/D channel 0 and write
 *   calculated data in mV/Ohm to display
 * -----------------------------------------
 */ 


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Hardware setup
//
//             +-----+
//             |  U  |
// +-----------+  S  +--------------+
// |     M     |  B  |  +---------+ |
// |     E     +-----+  | DISPLAY | +---+ -> VCC -------------------+
// |     G              +---------+ | P | -> PA0 (ADC0) <---+       |
// |     A                          | O |                   |      +++                |
// |     C                          | R |                   |      | | R1 = 1k        | U1
// |     A                          | T |                   |      | |                |
// |     R                          | A |                   |      +++                |
// |     D                          | / |                   |       |                \ /
// |                                | C |                   |       |                 *
// |                                |   |                   +-------+
// |                         +---+  +---+                           |                 |
// |                         | P |  |                               |                 |
// |                         +---+  +---+                          +++                |
// |                                | P |                          | | R2 = ?         | U2
// |                                | O |                          | |                |
// |                                | R |                          +++                |
// |                                | T |                           |                \ /
// |                                | B |                           |                 *
// |                                | / |                           |
// |                                | D |                           |
// |                                |   |                           |
// |                                +---+ -> GND -------------------+
// |     +-----+     +--------+     | 
// |     | ISP |     |  JTAG  |     |
// +-----+-----+-----+--------+-----+  
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define F_CPU 12000000UL    // System clock (required for delay)
#define R1 1000UL           // Resistance of R1


// System libraries
#include <avr/io.h>         // Required for I/O

// User libraries
#include "../../library/lcd/lcd.h"
#include "../../library/adc/adc.h"

int main(void)
{
    // Initialize LCD with defined parameters in lcd.h
    lcd_init();
    
    // Ohm character
    unsigned char pattern_ohm[] =   {
                                        0b00000,
                                        0b00000,
                                        0b01110,
                                        0b10001,
                                        0b10001,
                                        0b01010,
                                        0b01010,
                                        0b11011
                                    };
    
    lcd_pattern(LCD_CGADDR_CHAR0, pattern_ohm);
    
    // Initialize ADC with defined parameters in adc.h
    adc_init();
    
    // Select ADC channel 0 (unipolar mode)
    adc_channel(ADC_CH0);
    
    while (1) 
    {
        // Read data from A/D channel 0 and adjust it right
        unsigned int data = (adc_read()>>6);
        
        // Calculation the Voltage in mV
        //
        //                 5000 mV
        // U2_mV = data * --------- 
        //                  2^10
        //
        // Better (faster) option for dividing / 1024 is shifting 10 bits right (2^10)
        // unsigned int mv = (data * 1000UL) / 1024UL;
        unsigned int u2 = ((data * 5000UL)>>10);
        
        // Calculate the Resistor in Ohm
        //
        //      U1     U1
        // I = ---- = ----
        //      R1     R1
        //
        //       U2      U2      U2              data
        // R2 = ---- = ------ = ---- * R1 = ------------- * R1
        //       I       U1      U1          1024 - data
        //              ----
        //               R1
        //
        // Test:
        // R1 = 1000 Ohm, R2 = 470 Ohm
        //
        //            R2                    470 Ohm                  47
        // data = --------- * 1024 = -------------------- * 1024 = ----- * 1024 = ~327
        //         R1 + R2            1000 Ohm + 470 Ohm            147
        //
        //           327
        // R2 = ------------ * 1000 Ohm = ~469 Ohm
        //       1024 - 327
        //
        // Important for Calculation: (data * R1) before division!
        // Otherwise it needs to be a double calculation !!!
        unsigned int r2 = (data * R1) / (1024UL - data);
        
        // Write date to display
        // 1. Line
        lcd_home();
        // Write U=????mV to display
        lcd_string("U=");
        lcd_ul2ascii(u2, LCD_Decimal, 4);
        lcd_string("mV");
        
        // 2. Line
        lcd_cursor(0,1);
        // Write R=????Ohm to display
        lcd_string("R=");
        lcd_ul2ascii(r2, LCD_Decimal, 4);
        lcd_char(LCD_CGADDR_CHAR0);
    }
}