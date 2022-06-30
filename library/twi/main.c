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
 *   Sample for writing data over twi with
 *   twi library
 * -----------------------------------------
 */ 


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Hardware setup
//
//             +-----+
//             |  U  |
// +-----------+  S  +--------------+                          AT24C02
// |     M     |  B  |  +---------+ |                       +-----------+
// |     E     +-----+  | DISPLAY | +---+ -> VCC -- VCC <-  |     E     |
// |     G              +---------+ | P |                   |     E     |
// |     A                          | O |                   |     P     |
// |     C                          | R |                   |     R     |
// |     A                          | T |                   |     O     |
// |     R                          | A |                   |     M     |
// |     D                          | / |                   |           |
// |                                | C | -> PC6 -- SDA <-  |           |
// |                                |   | -> PC7 -- SCL <-  |           |
// |                                +---+ -> GND -- GND <-  |           |
// +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                       +-----------+
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define F_CPU 12000000UL    // System clock (required for delay)

// EEPROM settings
#define SLAVE_ADDR 0x42     // TWI_Slave address of the EEPROM
#define EEPROM_ADDR  0xF0   // EEPROM memory address
#define EEPROM_DATA  0xF0   // EEPROM test data

// System libraries
#include <avr/io.h>
#include <util/delay.h>

// User libraries
#include "twi.h"

int main(void)
{   
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!        INFORMATION           !!!
    // !!! If the pullup resistor on    !!!
    // !!! SCL/SDA has a very high      !!!
    // !!! resistance, it could be      !!!
    // !!! necessary to slow down the   !!!
    // !!! TWI clock speed or manually  !!!
    // !!! mount 10k resistors between  !!!
    // !!! SCL -> VCC and SDA -> VCC    !!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    DDRC |= (1<<PC7);   // Set PIN7 as output
    PORTC |= 0x03;      // Enable pullup resistors on SCL and SDA
    
        // Initialize TWI as TWI_Master
        twi_init(TWI_Master);
        
        // EEPROM WRITE routine
        twi_start();
        twi_address(SLAVE_ADDR, TWI_Write);
        twi_set(EEPROM_ADDR);
        twi_set(EEPROM_DATA);
        twi_stop();
        _delay_ms(5);
        
        while(1)
        {
            unsigned char data;
            
            // EEPROM READ routine
            twi_start();
            twi_address(SLAVE_ADDR, TWI_Write);
            twi_set(EEPROM_ADDR);
            twi_start();
            twi_address(SLAVE_ADDR, TWI_Read);
            twi_get(&data, TWI_NACK);
            twi_stop();
            
            // Check if data written to EEPROM is same as data that was red
            if(EEPROM_DATA == data)
                PORTC |= (1<<PC7);
        }           
}