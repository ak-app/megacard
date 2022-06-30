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
 *   Function file for uart library
 * -----------------------------------------
 */

#include "uart.h"

#if UART_STDMODE > 0

    // Initialize FILE stream
    #if !defined(UART_TXCIE) && !defined(UART_UDRIE) && !defined(UART_RXCIE) && UART_STDMODE == 1
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, uart_scanf, _FDEV_SETUP_RW);
    #elif !defined(UART_TXCIE) && !defined(UART_UDRIE) && UART_STDMODE == 2
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, NULL, _FDEV_SETUP_WRITE);
    #elif !defined(UART_RXCIE) && UART_STDMODE == 3
        static FILE std_uart = FDEV_SETUP_STREAM(NULL, uart_scanf, _FDEV_SETUP_READ);
    #endif

#endif

#if !defined(UART_RXCIE) && !defined(UART_TXCIE) && !defined(UART_UDRIE)
    #if UART_HANDSHAKE > 0
        static UART_Handshake uart_handshake_sending = UART_Ready;
    #endif
#endif

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart_init(void)
{
    // Check if hardware flow control is enabled
    #if UART_HANDSHAKE == 2
        // Setup RTS (output)/CTS (input)
        UART_HANDSHAKE_DDR |= (1<<UART_HANDSHAKE_RTS);
        UART_HANDSHAKE_DDR &= ~(1<<UART_HANDSHAKE_CTS);
    #endif
    
    // Check which bit sampling mode should be activated
    #if USE_2X
        UCSRA |= (1<<U2X);          // Setup 8 samples/bit
    #else
        UCSRA &= ~(1<<U2X);         // Setup 16 samples/bit
    #endif

    UBRRH = UBRRH_VALUE;            // Calculated through setbaud.h
    UBRRL = UBRRL_VALUE;            // Calculated through setbaud.h

    unsigned char SETREG = (1<<URSEL);  // Activate URSEL (normally in register UCSRC)
    
    SETREG |= (0x06 & ((UART_DATASIZE - 5)<<1));    // Setup data size
    
    #if UART_PARITY > 0
        SETREG |= (0x30 & ((UART_PARITY + 1)<<4));  // UART_Parity Mode
    #endif
    
    #if UART_STOPBITS > 1
        SETREG |= (0x08 & (UART_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSRC = SETREG;                 // Write SETREG settings to UCSRC
    UCSRB = (1<<RXEN) | (1<<TXEN);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART_RXCIE
        UCSRB |= (1<<RXCIE);
    #endif

    // Transmitter interrupt setup
    #if defined(UART_TXCIE) && !defined(UART_UDRIE)
        UCSRB |= (1<<TXCIE);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART_TXCIE) && defined(UART_UDRIE)
        UCSRB |= (1<<UDRIE);
    #endif

    #if !defined(UART_TXCIE) && !defined(UART_UDRIE) && (UART_STDMODE == 1 || UART_STDMODE == 2)
        stdout = &std_uart;
    #endif
    
    #if !defined(UART_RXCIE) && UART_STDMODE == 1 || UART_STDMODE == 3
        stdin = &std_uart;
    #endif
}

//  +---------------------------------------------------------------+
//  |                   UART send character                         |
//  +---------------------------------------------------------------+
//  |    Return:    UART_None=0  -> No error ocurred                |
//  |               UART_Frame   -> Error in frame                  |
//  |               UART_Overrun -> Data overrun @ transmission     |
//  |               UART_Parity  -> Parity error                    |
//  +---------------------------------------------------------------+
UART_Error uart_error_flags(void)
{
    // UART_Frame error
    if(UCSRA & (1<<FE))
    {
        UDR;           // Clear UART data register
        return UART_Frame;   // Return NUL
    }
    // Data UART_Overrun error
    else if(UCSRA & (1<<DOR))
    {
        UDR;           // Clear UART data register
        return UART_Overrun; // Return NUL
    }
    // UART_Parity error
    else if(UCSRA & (1<<UPE))
    {
        UDR;           // Clear UART data register
        return UART_Parity;  // Return NUL
    }
    return UART_None;
}

#if !defined(UART_TXCIE) && !defined(UART_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSRA & (1<<UDRE)));
        
        UDR = data; // Write data to transmission register
        
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2)
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x00    ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart_printf(char data, FILE *stream)
        {
            return uart_putchar(data);
        }
    #endif

#endif

#if !defined(UART_RXCIE)
    //  +---------------------------------------------------------------+
    //  |               UART receive character (non blocking)           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    0x??        -> Data buffer variable             |
    //  |                                                               |
    //  |    Return:    UART_Empty      -> No data in received          |
    //  |               UART_Received   -> Data received                |
    //  |               UART_Fault      -> Fault @ transmission         |
    //  +---------------------------------------------------------------+
    UART_Data uart_scanchar(char *data)
    {
        // If data has been received
        if((UCSRA & (1<<RXC)))
        {
            // Check if an UART_Error ocurred
            if(uart_error_flags() != UART_None)
            {
                UDR;           // Clear UDR0 Data register
                *data = 0;
                return UART_Fault;
            }
            
            #if UART_HANDSHAKE == 1
                if (*data == UART_HANDSHAKE_XON)
                {
                    uart_handshake_sending = UART_Ready;
                    return UART_Empty;
                }                
                else if (*data == UART_HANDSHAKE_XOFF)
                {
                    uart_handshake_sending = UART_Pause;
                    return UART_Empty;
                }                
            #endif
            
            *data = UDR;
            
            #if defined(UART_RXC_ECHO) && !defined(UART_TXCIE) && !defined(UART_UDRIE)
                // Send echo of received data to UART
                uart_putchar(*data);
            #endif
            
            return UART_Received;
        }
        return UART_Empty;
    }
    
    //  +---------------------------------------------------------------+
    //  |                   UART receive character                      |
    //  +---------------------------------------------------------------+
    //  | Parameter:    status (ptr) -> Pointer to return status        |
    //  |                            -> UART_Empty                      |
    //  |                            -> UART_Received                   |
    //  |                            -> UART_Error                      |
    //  |                                                               |
    //  |    Return:    0x??    ->  data/NUL                            |
    //  +---------------------------------------------------------------+
    char uart_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do
        {
            temp = uart_scanchar(&data);
        } while (temp == UART_Empty);
        
        *status = temp;
        return data;
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 3)
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart_scanf(FILE *stream)
        {
            return (int)uart_getchar(NULL);
        }

        //  +---------------------------------------------------------------+
        //  |                   UART error handler                          |
        //  +---------------------------------------------------------------+
        void uart_clear(void)
        {
            clearerr(stdin);    // Clear error on stream
            getchar();          // Remove character from stream
        }
    #endif

#endif

#if !defined(UART_RXCIE) && !defined(UART_TXCIE) && !defined(UART_UDRIE)
    #if UART_HANDSHAKE > 0
    //  +---------------------------------------------------------------+
    //  |                   UART handshake                              |
    //  +---------------------------------------------------------------+
    //  | Parameter:    status -> Ready                                 |
    //  |                         Pause                                 |
    //  +---------------------------------------------------------------+
    UART_Handshake uart_handshake(UART_Handshake status)
    {
        if(status == UART_Ready)
        {
            #if UART_HANDSHAKE == 1
                uart_putchar(UART_HANDSHAKE_XON);
            #elif UART_HANDSHAKE == 2
                UART_HANDSHAKE_PORT &= ~(1<<UART_HANDSHAKE_RTS);
            #endif
        }
        else if(status == UART_Pause)
        {
            #if UART_HANDSHAKE == 1
                uart_putchar(UART_HANDSHAKE_XOFF);
            #elif UART_HANDSHAKE == 2
                UART_HANDSHAKE_PORT |= (1<<UART_HANDSHAKE_RTS);
            #endif
        }
        else
        {
            #if UART_HANDSHAKE == 1
                return uart_handshake_sending;
            #elif UART_HANDSHAKE == 2
                if (!(UART_HANDSHAKE_PIN & (1<<UART_HANDSHAKE_CTS)))
                    return UART_Ready;
                return UART_Pause;
            #endif
        }
        return UART_Status;
    }
    #endif
#endif