/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: Megacard
 *    Hardware: ATmega16
 * -----------------------------------------
 *     Version: 2.0 Release
 *      Author: G.Raf
 * Description:
 *   Sample for writing data over uart with
 *   uart library
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

// User libraries
#include "uart.h"

// Static test variables for printf
static unsigned int x = 123;            // Numeric
static unsigned char a = 'A';           // Character
static unsigned char str[] = "Test";    // String
static double z = 123.43;               // Floating Point

int main(void)
{   
    // Initialize UART
    uart_init();

    // Print a String
    printf("Hello, world!\n\r");
    
    // Print different variables
    printf("Variables -> %d, %c, %s\n\r", x, a, str);
    
    // Linker parameter necessary:
    //  ->     Libraries: libprintf_flt
    //  ->       General: Use vprintf
    //  -> Miscellaneous: -Wl,-u,vfprintf -lprintf_flt -lm
    printf("Variable y: %f\n\n\r", z);

    while (1)
    {
        // Make a request from console
        printf("Please enter a number: ");
        unsigned int zahl1;
        
        if(scanf("%3u", &zahl1) == 1)
        {
            printf("\n\n\rThe result of %u * 5 equals: %u\n\n\r", zahl1, (zahl1 * 5));
        }
        else
        {
            uart_clear();
            printf("\n\rInput error!\n\n\r");
        }
        
        
        printf("Please insert 10 characters: ");
        char test[11];
        
        if(scanf("%10s", test) != 0)
        {
            printf("\n\n\rInput result: %s\n\n\r", test);
        }
        else
        {
            uart_clear();
            printf("\n\rInput error!\n\n\r");
        }
        
        printf("Please enter a decimal number: ");
        float zahl2;
        
        // Linker parameter necessary:
        //  ->     Libraries: libscanf_flt
        //  ->       General: Use vprintf
        //  -> Miscellaneous: -Wl,-u,vfscanf -lscanf_flt -lm
        if(scanf("%f", &zahl2) == 1)
        {
            printf("\n\n\rThe result of %f * 5.23 equals: %f\n\n\r", zahl2, (zahl2 * 5.23));
        }
        else
        {
            uart_clear();
            printf("\n\rInput error!\n\n\r");
        }
        
        _delay_ms(1000);
    }
}

