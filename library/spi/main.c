/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: Megacard/STK500/STK600
 *    Hardware: ATmega??-????
 * -----------------------------------------
 *     Version: 2.0 Release
 *      Author: G.Raf
 * Description:
 *   Sample for writing data over spi with
 *   spi library
 * -----------------------------------------
 */ 

#define F_CPU 12000000UL    // System clock (required for delay)

// System libraries
#include <avr/io.h>

// User libraries
#include "spi.h"

int main(void)
{   
    // Port setup
    DDRC = 0xFF;    // Set PORT as output
    DDRA = 0x00;    // Set PORT as input
    PORTA = 0x0F;   // SPI_Enable pullup resistor
    
    // SPI Initialization
    //      Mode: SPI_Master
    // Direction: SPI_MSB
    //  Transfer: SPI_Rising/SPI_Rising
    //
    // Details can be found in spi.c
    spi_init(SPI_Master, SPI_MSB, SPI_Rising, SPI_Rising);
    
    while (1) 
    {
        // This is a simple loopback mode (connect MOSI to MISO)
        // to check the function of the SPI bus. There are 4 Switches
        // connected to PORTA (3:0). If one or more switches are
        // active the status LEDs on PORTC should remain the status
        // of the switches. If the connection between MOSI and MISO
        // is removed the status LEDs should all be HIGH (active)
        // because of the activated pullup on MISO!!!
        unsigned char data = spi_transfer(~((0xF0 & (PINA << 4)) | (0x0F & PINA)));
        
        PORTC = data;   // Write received data to output
    }   // End of loop
}   // End of main
