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

/** Initialise the UART
 */
void uartInit() {
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
  return 0x00;
  }

/** Write a single character
 *
 * Send a single character on the UART.
 *
 * @param ch the character to send.
 */
void uartSend(char ch) {
  }
