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
 *   Function file for spi library
 * -----------------------------------------
 */

#include "spi.h"

//  +---------------------------------------------------------------+
//  |                   SPI initialization                          |
//  +---------------------------------------------------------------+
//  | Parameter:    operation   ->  0x01 = Master mode              |
//  |                               0x00 = Slave mode               |
//  |                                                               |
//  |               direction   ->  0x01 = LSB first                |
//  |                               0x00 = MSB first                |
//  |                                                               |
//  |                               +------+----------+---------+   |
//  |                               |      | Polarity | Phase   |   |
//  |                               +------+----------+---------+   |
//  |               transfer    ->  | 0x00 | Rising   | Rising  |   |
//  |                               | 0x01 | Rising   | Falling |   |
//  |                               | 0x02 | Falling  | Rising  |   |
//  |                               | 0x03 | Falling  | Falling |   |
//  |                               +------+----------+---------+   |
//  |                                                               |
//  |    Return:    0x00    ->  Init complete                       |
//  |               0xFF    ->  Master abort                        |
//  +---------------------------------------------------------------+
unsigned char spi_init(SPI_Mode operation, SPI_Direction direction, SPI_Polarity polarity, SPI_Phase phase)
{   
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!      SPECIAL FUNCTION        !!!
    // !!! If during initialisation SS  !!!
    // !!! pin is LOW, SPI controller   !!!
    // !!! will be configured as slave. !!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SPI_DDR  &= ~((1<<SPI_MISO) | (1<<SPI_SS)); // Setup MISO and SS as input
    SPI_PORT |=   (1<<SPI_MISO) | (1<<SPI_SS);  // Activate pull up resistor at MISO and SS
    
    // Double speed setup
    #ifdef SPI2X
        SPSR = (1<<SPI2X);                      // Double speed mode activated
    #else
        SPSR = 0x00;                            // Double speed mode deactivated
    #endif
    
    #if SPI_CLOCK > 0
        SPCR = (0x03 & SPI_CLOCK);
    #else
        SPCR = 0x00;
    #endif
    
    // Master/Slave setup (direction setup)
    SPCR |= ((0x01 & operation)<<4);
    
    // MSB/LSB first
    SPCR |= ((0x01 & direction)<<5);
    
    // Polarity and Phase of SCK and DATA
    SPCR |= ((0x04 & polarity)<<3) | ((0x02 & phase)<<2);
    
    // SPI interrupt setup
    #ifdef SPI_SPIE
        SPCR  |= (1<<SPIE);
        sei();
    #endif
    
    SPCR |= (1<<SPE);   // Activate the SPI Controller
    
    // Setup SCK, MOSI and SS as output
    // PORT configuration gets overwritten from SPI controller
    SPI_DDR  |= (1<<SPI_SCK) | (1<<SPI_MISO) | (1<<SPI_MOSI) | (1<<SPI_SS);
    
    // Check if master abort has occurred
    if(!(SPCR & (1<<MSTR)))
        return 0xFF;    // Return master abort
    return 0x00;        // Return no fault
}

//  +---------------------------------------------------------------+
//  |                   SPI disable function                        |
//  +---------------------------------------------------------------+
void spi_disable(void)
{
    // Disable SPI
    SPCR &= ~(1<<SPE);
    SPI_DDR  &= ~((1<<SPI_SCK) | (1<<SPI_MOSI) | (1<<SPI_MISO) | (1<<SPI_SS));
    SPI_PORT &= ~((1<<SPI_SCK) | (1<<SPI_MOSI) | (1<<SPI_MISO) | (1<<SPI_SS));
    
    #ifdef SPI_SPIE // Disable SPI interrupt
        SPCR  &= ~(1<<SPIE);
        sei();
    #endif
}

//  +---------------------------------------------------------------+
//  |               SPI Master Slave Select                         |
//  +---------------------------------------------------------------+
//  | Parameter:    mode    ->  0x01 = Slave Select Pin HIGH -> LOW |
//  |                           0x00 = Slave Select Pin LOW -> HIGH |
//  +---------------------------------------------------------------+
void spi_select(SPI_Select mode)
{
    switch(mode)
    {
        case SPI_Enable : SPI_PORT &= ~(1<<SPI_SS);   break;  // Slave Select On
        default         : SPI_PORT |=  (1<<SPI_SS);   break;  // Slave Select Off
    }
}

//  +---------------------------------------------------------------+
//  |               SPI Slave Slave Select                          |
//  +---------------------------------------------------------------+
//  |    Return:    0x00    ->  Slave disabled                      |
//  |               0xFF    ->  Slave enabled                       |
//  +---------------------------------------------------------------+
SPI_Select spi_slave_select(void)
{
    if(!(SPI_PIN & (1<<SPI_SS)))
        return SPI_Enable;
    else
        return SPI_Disable;
}

#ifndef SPI_SPIE

    //  +---------------------------------------------------------------+
    //  |           SPI master transfer / receive character             |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  Transmit data byte                  |
    //  |                                                               |
    //  |    Return:    0x??    ->  Receive data byte                   |
    //  +---------------------------------------------------------------+
    unsigned char spi_transfer(unsigned char data)
    {
        SPDR = data;    // Write data into the SPI Data Register and initiate a transmission
    
        // Wait until transmission is Complete
        while(!(SPSR & (1<<SPIF)))
            asm volatile("NOP");
        
        #ifdef SPI_WCOL_PORT
            #ifdef SPI_WCOL_PIN
                // Write Collision to Collision Port (Sticky) 
                if(SPSR & (1<<WCOL))
                    SPI_WCOL_PORT |= (1<<SPI_WCOL_PIN);
            #endif
        #endif
        
        return SPDR;    // Return received data from the bus
    }

    //  +---------------------------------------------------------------+
    //  |           SPI master transfer / receive character             |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  Transmit/Receive data byte          |
    //  |                                                               |
    //  |    Return:    Received  ->  Data received                     |
    //  |               Collision ->  Data collision                    |
    //  |               None      ->  No data received                  |
    //  +---------------------------------------------------------------+
    SPI_Status spi_slave_transfer(unsigned char *data)
    {   
        if(SPSR & (1<<SPIF))
        {
            unsigned char temp = *data;  // Write data into a temporary buffer
            
            *data = SPDR;   // Write data form buffer into data variable
            SPDR = temp;    // Setup new data for next transmission and reset WCOL and SPIF
            
            // Check if this is necessary
            // Write Collision to Collision Port (Sticky)
            if(SPSR & (1<<WCOL))
            {
                #ifdef SPI_WCOL_PORT
                    #ifdef SPI_WCOL_PIN
                        SPI_WCOL_PORT |= (1<<SPI_WCOL_PIN);
                    #endif
                #endif
                
                SPDR = temp;            // Write data again into the SPI data register and reset WCOL and SPIF
                return SPI_Collision;   // Return that a collision happened
            }
            return SPI_Received;        // Return that new data received
        }
        return SPI_None;                // Return that no new data received
    }
#endif
