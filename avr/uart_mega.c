/*--------------------------------------------------------------------------*
* UART interface implementation for ATmega
*---------------------------------------------------------------------------*
* 01-Apr-2014 ShaneG
*
*  Initial version.
*--------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "hardware.h"
#include "microboot.h"

// AVR-GCC compiler compatibility
// avr-gcc compiler v3.1.x and older doesn't support outb() and inb()
#ifndef outb
  #define outb(sfr,val)  (_SFR_BYTE(sfr) = (val))
#endif
#ifndef inb
  #define inb(sfr) _SFR_BYTE(sfr)
#endif

/** Initialise the UART
 */
void uartInit() {
#ifdef __AVR_ATmega8__
  UBRRH = (((F_CPU/BAUD_RATE)/16)-1)>>8;     // set baud rate
  UBRRL = (((F_CPU/BAUD_RATE)/16)-1);
  UCSRB = (1<<RXEN)|(1<<TXEN);               // enable Rx & Tx
  UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // config USART; 8N1
#else
  UBRR0H = (((F_CPU/BAUD_RATE)/16)-1)>>8;    // set baud rate
  UBRR0L = (((F_CPU/BAUD_RATE)/16)-1);
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);            // enable Rx & Tx
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);          // config USART; 8N1
#endif
  }

/** Shutdown the UART
 */
void uartDone() {
  }

/** Receive a single character
 *
 * Wait for a single character on the UART and return it.
 *
 * @return the character received.
 */
char uartRecv() {
#ifdef __AVR_ATmega8__
  /* Wait for data to be received */
  while(!(UCSRA&(1<<RXC)));
  return (char)UDR;
#else
  while(!(inb(UCSR0A) & _BV(RXC0)));
  return (inb(UDR0));
#endif
  }

/** Write a single character
 *
 * Send a single character on the UART.
 *
 * @param ch the character to send.
 */
void uartSend(char ch) {
#ifdef __AVR_ATmega8__
  while(!(UCSRA&(1<<UDRE)));
  UDR = ch;
#else
  while (!(inb(UCSR0A) & _BV(UDRE0)));
  outb(UDR0,ch);
#endif
  }

