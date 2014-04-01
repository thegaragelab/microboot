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
  UBRRH = (((F_CPU/BAUD_RATE)/16)-1)>>8;     // set baud rate
  UBRRL = (((F_CPU/BAUD_RATE)/16)-1);
  UCSRB = (1<<RXEN)|(1<<TXEN);               // enable Rx & Tx
  UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // config USART; 8N1
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
  while(!(inb(UCSRA) & _BV(RXC)));
  return (inb(UDR));
  }

/** Write a single character
 *
 * Send a single character on the UART.
 *
 * @param ch the character to send.
 */
void uartSend(char ch) {
  while (!(inb(UCSRA) & _BV(UDRE)));
  outb(UDR,ch);
  }

