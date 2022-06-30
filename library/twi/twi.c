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
 *   Function file for twi library
 * -----------------------------------------
 */

#include "twi.h"

//  +---------------------------------------------------------------+
//  |                   TWI initialization                          |
//  +---------------------------------------------------------------+
//  | Parameter:    operation   ->  0x01 = Master mode              |
//  |                               0x00 = Slave mode               |
//  |                                                               |
//  |    Return:    BIT(7:3)    ->  TWS7 - TWS3                     |
//  |               BIT(2)      ->  TWWC                            |
//  |               BIT(1:0)    ->  0b00                            |
//  +---------------------------------------------------------------+
unsigned char twi_init(TWI_Mode operation)
{
    // Set Slave Address and general Call
    TWAR = ((unsigned char)(((unsigned char)(TWI_ADDRESS))<<1)) | (0x01 & ((unsigned char)(TWI_BROADCAST)));
    
    // Master / Slave setup
    switch(operation)
    {
        case TWI_Slave :    // Slave Mode
                    TWSR &= ~(0x03);                        // Reset TWI Prescaler
                    TWCR = (1<<TWEA) | (1<<TWEN);           // Enable TWI Bus and Acknowledge to TWI_ADDR or general call
                    break;
        default :           // Master mode
                    TWBR = (unsigned char)(TWI_BITRATE);    // Setup TWI Bitrate
                    TWSR = (unsigned char)(TWI_PRESCALE);   // Setup TWI Prescaler
                    break;
    }
    
    // TWI interrupt setup
    #ifdef TWI_TWIE
        TWCR  |= (1<<TWIE);
        sei();
    #endif

    // Return message initialization
    return twi_status();
}

//  +---------------------------------------------------------------+
//  |                   TWI disable function                        |
//  +---------------------------------------------------------------+
void twi_disable(void)
{
    // Disable TWI
    TWCR &= ~((1<<TWEA) | (1<<TWEN) | (1<<TWIE));
    
    #ifdef TWI_TWIE // Diable TWI interrupt
        TWCR  &= ~(1<<TWIE);
    #endif
}

//  +---------------------------------------------------------------+
//  |                       TWI status byte                         |
//  +---------------------------------------------------------------+
//  |    Return:    BIT(7:3)    ->  TWS7 - TWS3                     |
//  |               BIT(2)      ->  TWWC                            |
//  |               BIT(1:0)    ->  0b00                            |
//  +---------------------------------------------------------------+
unsigned char twi_status(void)
{
    return (0xF8 & TWSR) | (0x04 & (TWCR>>1));
}

#ifndef TWI_TWIE

    //  +---------------------------------------------------------------+
    //  |                   TWI start transmission                      |
    //  +---------------------------------------------------------------+
    //  |    Return:    TWI_None        ->  Start successful            |
    //  |               TWI_Start       ->  Start failure               |
    //  +---------------------------------------------------------------+
    TWI_Error twi_start(void)
    {
        TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Start a TWI transmission
                            
        // Check if START done
        while(!(TWCR & (1<<TWINT)))
            asm volatile("NOP");
    
        // Check if an error occurred
        if(((TWSR & 0xF8) == TWI_STATUS_START) || ((TWSR & 0xF8) == TWI_STATUS_REPEATED_START))
            return TWI_None;    // (Repeated)Start successfully
        return TWI_Start;       // (Repeated)Start failure
    }

    //  +---------------------------------------------------------------+
    //  |                   TWI stop transmission                       |
    //  +---------------------------------------------------------------+
    void twi_stop(void)
    {
        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); // Stop a TWI transmission
    
        while(!(TWCR & (1<<TWSTO)))
            asm volatile("NOP");
    }

    //  +---------------------------------------------------------------+
    //  |               TWI address + read/write data                   |
    //  +---------------------------------------------------------------+
    //  | Parameter:    address     ->  Adress (Bit6:0) of slave device |
    //  |               operation   ->  WRITE/READ operation            |
    //  |                                                               |
    //  |    Return:    TWI_None        ->  Addressing successful       |
    //  |               TWI_Arbitration ->  Arbitration lost (m. master)|
    //  |               TWI_General     ->  Error occured               |
    //  +---------------------------------------------------------------+
    TWI_Error twi_address(unsigned char address, TWI_Operation operation)
    {
        TWDR = (address<<1) | (0x01 & operation);   // Write data to data register
        TWCR = (1<<TWINT) | (1<<TWEN);              // Transmit address byte + operation
    
        // Check if transmission done
        while(!(TWCR & (1<<TWINT)))
            asm volatile("NOP");
    
        // ADDRESS + WRITE operation
        if(operation == TWI_Write)
        {
            if(((TWSR & 0xF8) == TWI_STATUS_ADDRESS_WRITE_ACK) || ((TWSR & 0xF8) == TWI_STATUS_ADDRESS_WRITE_NACK))
                return TWI_None;
        }
        // ADDRESS + READ operation
        else
        {
            if(((TWSR & 0xF8) == TWI_STATUS_ADDRESS_READ_ACK) || ((TWSR & 0xF8) == TWI_STATUS_ADDRESS_READ_NACK))
                return TWI_None;
        }
        
        // Check if arbitration lost (only multi-master mode)
        if((TWSR & 0xF8) == TWI_STATUS_ARBITRATION_LOST)
            return TWI_Arbitration;
        
        return TWI_General;
    }

    //  +---------------------------------------------------------------+
    //  |                   TWI transmit data                           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data        ->  Data to transmit                |
    //  |                                                               |
    //  |    Return:    TWI_None        ->  Data transmitted successful |
    //  |               TWI_Arbitration ->  Arbitration lost (m. master)|
    //  |               TWI_General     ->  Error occured               |
    //  +---------------------------------------------------------------+
    TWI_Error twi_set(unsigned char data)
    {
        TWDR = data;                                // Write data to data register
        TWCR = (1<<TWINT) | (1<<TWEN);              // Transmit databyte
    
        // Check if transmission done
        while(!(TWCR & (1<<TWINT)))
            asm volatile("NOP");
    
        // Check if an error occurred
        if(((TWSR & 0xF8) == TWI_STATUS_DATA_WRITE_ACK) || ((TWSR & 0xF8) == TWI_STATUS_DATA_WRITE_NACK))
            return TWI_None;
        
        // Check if arbitration lost (only multi-master mode)
        if((TWSR & 0xF8) == TWI_STATUS_ARBITRATION_LOST)
            return TWI_Arbitration;
        
        return TWI_General;
    }

    //  +---------------------------------------------------------------+
    //  |                   TWI receive data                            |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data        ->  Received data                   |
    //  |               acknowledge ->  TWI_ACK/TWI_NACK                |
    //  |                                                               |
    //  |    Return:    TWI_None        ->  Data received successful    |
    //  |               TWI_Acknowledge ->  Wrong ACK/NACK parameter    |
    //  |               TWI_Arbitration ->  Arbitration lost (m. master)|
    //  |               TWI_General     ->  Error occured               |
    //  +---------------------------------------------------------------+
    TWI_Error twi_get(unsigned char *data, TWI_Acknowledge acknowledge)
    {
        // Check if ACK should be sent
        if(acknowledge == TWI_ACK)
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);  // Transmit databyte + ACK
        // Check if NACK should be sent
        else if(acknowledge == TWI_NACK)
            TWCR = (1<<TWINT) | (1<<TWEN);              // Transmit databyte + NACK
        else
            return TWI_Ack;
    
        // Check if transmission done
        while (!(TWCR & (1<<TWINT)))
            asm volatile("NOP");
    
        // TWI_Write Data to pointer
        *data = TWDR;
    
        // Check if an error occurred
        if(((TWSR & 0xF8) == TWI_STATUS_DATA_READ_ACK) || ((TWSR & 0xF8) == TWI_STATUS_DATA_READ_NACK))
            return TWI_None;
        
        // Check if arbitration lost (only multi-master mode)
        if((TWSR & 0xF8) == TWI_STATUS_ARBITRATION_LOST)
            return TWI_Arbitration;
        
        return TWI_General;
    }

#endif