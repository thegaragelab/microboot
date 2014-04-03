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

// Define ports for the virtual serial port
// Use command line options -W 0x20,- -R 0x22,-
#define UART_OUT (*((volatile char *)0x20))
#define UART_IN  (*((volatile char *)0x22))

/** Initialise the UART
 */
void uartInit() {
  // Do nothing
  }

/** Shutdown the UART
 */
void uartDone() {
  // Do nothing
  }

/** Receive a single character
 *
 * Wait for a single character on the UART and return it.
 *
 * @return the character received.
 */
char uartRecv() {
  return UART_IN;
  }

/** Write a single character
 *
 * Send a single character on the UART.
 *
 * @param ch the character to send.
 */
void uartSend(char ch) {
  UART_OUT = ch;
  }

