/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 2.0 Release
 *      Author: G.Raf
 * Description:
 *   Read data from uart, sort data and
 *   return sorted data over uart
 * -----------------------------------------
 */


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Hardware setup
//
// +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                                                     +-----------------------------+
// |     M                          |                                                     |                             |
// |     E                          |                       +-----------+                 | +-----------------------+   |
// |     G                          +---+                   |   U   +---+---+             | | TeraTerm              |   |
// |     A                          | P |                   |   S   |  USB  |-------------+ | ~~~~~~~~              |   |
// |     C                          | O |                   |   B   +---+---+             | | ~~~~~                 |   |
// |     A                          | R |                   |   /       |                 | | ~~~~~~~~~~~~          |   |
// |     R                          | T |                   |   U       |                 | |                       |   |
// |     D                          | B |                   |   A       |                 | +-----------------------+   |
// |                                | / |                   |   R       |                 |                             |
// +-----+                          | D | -> PD1 -- TXD <-  |   T       |                 | Computer                    |
// | X1  |                          |   | -> PD0 -- RXD <-  |  +--------+                 |                             |
// +-----+                          +---+ -> GND -- GND <-  |  | 5V/3V3 |                 |                             |
// |     +-----+     +--------+     |                       +--+--------+                 |                             |
// |     | ISP |     |  JTAG  |     |                                                     |                             |
// +-----+-----+-----+--------+-----+                                                     +-----------------------------+
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// TeraTerm (COM?|8N1@9600)
//
// ----------------------CONSOLE I/O----------------------
// Length[2-200]: a Input error!
// Length[2-200]: 1
// Value < 2 || Value > 200!
//
// Length[2-200]: 201
// Value < 2 || Value > 200!
//
// Length[2-200]: 10
//
// Data[0]: a      Input error!
// Data[0]: 99999
// Data[1]: 83736
// Data[2]: 123
// Data[3]: 3421
// Data[4]: 193
// Data[5]: 12930
// Data[6]: 92836
// Data[7]: 76453
// Data[8]: 12343
// Data[9]: 21234
//
// {  123,   193,  3421, 12343, 12930, 21234, 76453, 83736, 92836, 99999, }
//
// End!
//
// -------------------------------------------------------
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define F_CPU 12000000UL    // System clock (required for uart)
#define MAXLENGTH 200        // Maximum length of data array

// System libraries
#include <avr/io.h>         // Required for I/O

// User libraries
#include "../../library/uart/uart.h"

static unsigned long data[MAXLENGTH];

int main(void)
{
    // Port setup
    DDRC = 0xFF;    // Set PORT as output
    
    // Initialize UART(8N1) with defined parameters in uart.h
    uart_init();
    
    // Clear console
    printf("\033[2J");
    
    while (1)
    {
        printf("\n\rLength[2-%u]: ", (unsigned char)(MAXLENGTH));
        
        unsigned int length = 0;
        
        // Read length value
        if(scanf("%3u", &length) == 1)
        {
            // Check if length value is in specified range
            if(length < 2 || length > MAXLENGTH)
            {
                printf("\n\rValue < 2 || Value > %u!\n\r", (unsigned int)(MAXLENGTH));
            }
            else
            {
                printf("\n\n\r");
                
                // Read data
                for (unsigned char i=0; i < length; i++)
                {
                    printf("Data[%u]: ", i);
                
                    // Allowed input [0 - 99999]
                    if(!(scanf("%5lu", &data[i]) == 1))
                    {
                        uart_clear();
                        printf("\tInput error!");
                        i--;
                    }
                
                    printf("\n\r");
                }
                
                // Bubble Sort algorithm (https://en.wikipedia.org/wiki/Bubble_sort)
                unsigned char loop;
            
                do
                {
                    loop = 0;
                
                    for (unsigned char i=0; i < (length - 1); i++)
                    {
                        if(data[i] > data[(i + 1)])
                        {
                            unsigned long temp = data[i];
                            data[i] = data[(i + 1)];
                            data[(i + 1)] = temp;
                        
                            loop = 1;
                        }
                    }
                } while (loop);
                
                // Result as sets
                printf("\n\r{");
                
                for (unsigned char i = 0; i < length; i++)
                {
                    printf("%5lu, ", data[i]);
                }
                
                printf("}\n\n\rEnd!\n\n\r");   
            }
        }
        else
        {
            uart_clear();
            printf("\tInput error!");
        }
    }
}