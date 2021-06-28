/* -----------------------------------------
 * Liebherr Lehrlingsausbildung
 * www.liebherr.com
 * -----------------------------------------
 *    Hardware: Megacard (ATmega16)
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Header file for uart library
 * -----------------------------------------
 */

#ifndef UART_H_
#define UART_H_

#ifndef F_CPU               // System clock
    #define F_CPU 12000000UL
#endif


#ifndef BAUD                // Transmission frequency in bits/s
    #define BAUD 9600UL
#endif


#ifndef UART_DATASIZE       // Setup 5 to 8
    #define UART_DATASIZE 8
#endif


#ifndef UART_PARITY         // Setup 0 = none | 1 = even | 2 = odd
    #define UART_PARITY 0
#endif


#ifndef UART_STOPBITS       // Setup 1 - 2
    #define UART_STOPBITS 1
#endif

#ifndef UART_RXC_ECHO       // Setup if echo occurs on data receiving
    #define UART_RXC_ECHO
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UARTRXCIE // Receiver Interrupt based processing
//  #define UARTRXCIE
//#endif

//#ifndef UARTTXCIE // Transmitter Interrupt based processing
//  #define UARTTXCIE
//#endif

//#ifndef UARTUDRIE // Transmitter Interrupt based processing
//  #ifndef UARTTXCIE
//      #define UARTUDRIE
//  #else
//      #undef UARTUDRIE
//  #endif
//#endif

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>

#if defined(UARTRXCIE) || defined(UARTTXCIE) || defined(UARTUDRIE)
    #include <avr/interrupt.h>
#endif

void uart_init(void);

#if !defined(UARTTXCIE) && !defined(UARTUDRIE)
     int uart_putchar(char c, FILE *stream);
#endif

#if !defined(UARTRXCIE)
     int uart_getchar(FILE *stream);
#endif

#if !defined(UARTRXCIE)
    void uart_error(void);
#endif

#endif /* UART_H_ */