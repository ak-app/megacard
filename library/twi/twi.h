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
 *   Header file for twi library
 * -----------------------------------------
 */

#ifndef TWI_H_
#define TWI_H_

#ifndef F_CPU               // System clock
    #define F_CPU 12000000UL
#endif

// Definition of TWI parameters
#ifndef F_TWI               // TWI clock
    #define F_TWI 100000UL
#endif  

#ifndef TWI_PRESCALE        // TWI TWSR register prescaler (0-3)
    #define TWI_PRESCALE 0
#endif

#ifndef TWI_BITRATE         // Bitrate register
    #ifdef TWI_PRESCALE
        #define TWI_BITRATE ((F_CPU/F_TWI) - 16UL) / (2 * (1<<TWI_PRESCALE) * (1<<TWI_PRESCALE))
    #endif
#endif

#ifndef TWI_ADDRESS         // Slave address of the controller
    #define TWI_ADDRESS 0x42
#endif

#ifndef TWI_BROADCAST       // Listen to general calls (0 = false/1 = true)
    #define TWI_BROADCAST 0x00
#endif

#ifndef TWI_ACKNOWLEDGE     // TWI acknowledge flag
    #define TWI_ACKNOWLEDGE 0x01
#endif

#ifndef TWI_NACKNOWLEDGE    // TWI not acknowledge flag
    #define TWI_NACKNOWLEDGE 0x00
#endif

#ifndef TWI_WRITE           // TWI write command
    #define TWI_WRITE 0x00
#endif

#ifndef TWI_READ            // TWI read command
    #define TWI_READ 0x01
#endif

// ATmega status codes (!!! Do not change !!!)
#define TWI_STATUS_START                0x08
#define TWI_STATUS_REPEATED_START       0x10
#define TWI_STATUS_ADDRESS_WRITE_NACK   0x18
#define TWI_STATUS_ADDRESS_WRITE_ACK    0x20
#define TWI_STATUS_DATA_WRITE_NACK      0x28
#define TWI_STATUS_DATA_WRITE_ACK       0x30
#define TWI_STATUS_ARBITRATION_LOST     0x38
#define TWI_STATUS_ADDRESS_READ_ACK     0x40
#define TWI_STATUS_ADDRESS_READ_NACK    0x48
#define TWI_STATUS_DATA_READ_ACK        0x50
#define TWI_STATUS_DATA_READ_NACK       0x58

// Definition of TWI processing (Enabled = Interrupt/Disabled = Polling)

//  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  !!! Interrupts are not included in this library  !!!
//  !!! If interrupts are used, polling will be      !!!
//  !!! disabled                                     !!!
//  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef TWI_TWIE  // TWI Interrupt based processing
//  #define TWI_TWIE
//#endif

#include <avr/io.h>
#include <avr/interrupt.h>

enum TWI_Mode_t
{
  TWI_Slave=0,
  TWI_Master=1  
};
typedef enum TWI_Mode_t TWI_Mode;

enum TWI_Operation_t
{
    TWI_Write=TWI_WRITE,
    TWI_Read=TWI_READ
};
typedef enum TWI_Operation_t TWI_Operation;

enum TWI_Acknowledge_t
{
    TWI_NACK=TWI_NACKNOWLEDGE,
    TWI_ACK=TWI_ACKNOWLEDGE
};
typedef enum TWI_Acknowledge_t TWI_Acknowledge;

enum TWI_Error_t
{
    TWI_None=0,
    TWI_Start,
    TWI_Arbitration,
    TWI_Ack,
    TWI_General
};
typedef enum TWI_Error_t TWI_Error;

unsigned char twi_init(TWI_Mode operation);
         void twi_disable(void);
unsigned char twi_status(void);

#ifndef TWI_TWIE
    TWI_Error twi_start(void);
         void twi_stop(void);
    TWI_Error twi_address(unsigned char address, TWI_Operation operation);
    TWI_Error twi_set(unsigned char data);
    TWI_Error twi_get(unsigned char *data, TWI_Acknowledge acknowledge);
#endif


#endif /* TWI_H_ */