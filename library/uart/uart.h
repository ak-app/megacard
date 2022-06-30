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
 *   Header file for uart library
 * -----------------------------------------
 */

#ifndef UART_H_
#define UART_H_

#ifndef F_CPU                       // System clock
    #define F_CPU 12000000UL
#endif

#ifndef BAUD                        // Transmission frequency in bits/s
    #define BAUD 9600UL
#endif

#ifndef UART_DATASIZE               // Setup 5 to 8
    #define UART_DATASIZE 8
#endif

#ifndef UART_PARITY                 // Setup 0 = none | 1 = even | 2 = odd
    #define UART_PARITY 0
#endif

#ifndef UART_STOPBITS               // Setup 1 - 2
    #define UART_STOPBITS 1
#endif

#ifndef UART_RXC_ECHO               // Setup that an echo occurs on data receiving
    #define UART_RXC_ECHO           // (disabled if UART_TXCIE or UART_UDRIE is set)
#endif

#ifndef UART_HANDSHAKE              // Handshake between communication systems
    #define UART_HANDSHAKE 1        // 1 = Software (XON/XOFF)
                                    // 2 = Hardware
                                    
    #if UART_HANDSHAKE == 2         // PORT settings for hardware handshake
        #ifndef UART_HANDSHAKE_DDR
            #define UART_HANDSHAKE_DDR  DDRC
        #endif
        #ifndef UART_HANDSHAKE_PORT
            #define UART_HANDSHAKE_PORT PORTC
        #endif
        #ifndef UART_HANDSHAKE_PIN
            #define UART_HANDSHAKE_PIN  PINC
        #endif
        #ifndef UART_HANDSHAKE_CTS
            #define UART_HANDSHAKE_CTS  PINC0
        #endif
        #ifndef UART_HANDSHAKE_RTS
            #define UART_HANDSHAKE_RTS  PINC1
        #endif
    #endif
    
    #ifndef UART_HANDSHAKE_XON
        #define UART_HANDSHAKE_XON 0x11
    #endif
    #ifndef UART_HANDSHAKE_XOFF
        #define UART_HANDSHAKE_XOFF 0x13
    #endif
#endif

// Definition of UART standard output mode (printf/scanf)
#ifndef UART_STDMODE                // Standard Mode
    #define UART_STDMODE 1          // 0 = None
                                    // 1 = printf/scanf
                                    // 2 = printf
                                    // 3 = scanf
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UART_RXCIE  // Receiver Interrupt based processing
  //#define UART_RXCIE
//#endif
//
//#ifndef UART_TXCIE  // Transmitter Interrupt based processing
  //#define UART_TXCIE
//#endif
//
//#ifndef UART_UDRIE  // Transmitter Interrupt based processing
  //#ifndef UART_TXCIE
      //#define UART_UDRIE
  //#else
      //#error "UART_TXCIE defined"
  //#endif
//#endif

enum UART_Data_t
{
    UART_Empty=0,
    UART_Received,
    UART_Fault
};
typedef enum UART_Data_t UART_Data;

enum UART_Error_t
{
    UART_None=0,
    UART_Frame,
    UART_Overrun,
    UART_Parity
};
typedef enum UART_Error_t UART_Error;

enum UART_Handshake_t
{
    UART_Status=0,
    UART_Ready,
    UART_Pause
};
typedef enum UART_Handshake_t UART_Handshake;

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>

#if defined(UART_RXCIE) || defined(UART_TXCIE) || defined(UART_UDRIE)
    #include <avr/interrupt.h>
#endif

      void uart_init(void);
UART_Error uart_error_flags(void);

#if !defined(UART_TXCIE) && !defined(UART_UDRIE)
    char uart_putchar(char data);
        
    #if UART_STDMODE == 1 || UART_STDMODE == 2
        int uart_printf(char data, FILE *stream);
    #endif
#endif

#if !defined(UART_RXCIE)
         char uart_getchar(UART_Data *status);
    UART_Data uart_scanchar(char *data);
        
    #if UART_STDMODE == 1 || UART_STDMODE == 3
             int uart_scanf(FILE *stream);
            void uart_clear(void);
    #endif
#endif

#if !defined(UART_TXCIE) && !defined(UART_UDRIE) && !defined(UART_RXCIE)
    #if UART_HANDSHAKE > 0
        UART_Handshake uart_handshake(UART_Handshake status);
    #endif
#endif

#endif /* UART_H_ */