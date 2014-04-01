/*--------------------------------------------------------------------------*
* Microboot implementation for ATmega8/88/168 chips
*---------------------------------------------------------------------------*
* 01-Apr-2014 ShaneG
*
*  Initial version.
*--------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

// Seed value for checksum
#define CHECKSUM_SEED    0x5050

// Protocol character indicators
#define OK   '+'
#define FAIL '-'
#define EOL  '\n'

// Packet sizes
#define DATA_SIZE        16
#define ADDRESS_SIZE     2
#define CHECKSUM_SIZE    2
#define READ_BYTES       (ADDRESS_SIZE + CHECKSUM_SIZE)
#define WRITE_BYTES      (ADDRESS_SIZE + DATA_SIZE + CHECKSUM_SIZE)
#define BUFFER_SIZE      (ADDRESS_SIZE + DATA_SIZE + CHECKSUM_SIZE)

// Identification information (protocol & CPU)
#define PROTOCOL_VERSION 0x10
#define CPU_TYPE         0x01
#define CPU_MODEL        0x02

uint8_t g_buffer[BUFFER_SIZE];

/** Determine if bootloader entry is required.
 *
 * @return true if the bootloader entry pin is low.
 */
inline bool bootLoaderRequired() {
  return true;
  }

/** Calculate a checksum
 */
inline uint16_t checksum(uint16_t total, uint8_t data) {
  return total + (uint16_t)data;
  }

/** Initialise the UART
 */
inline void uartInit() {
  }

/** Shutdown the UART
 */
inline void uartDone() {
  }

/** Receive a single character
 */
char uartRecv() {
  return 0x00;
  }

/** Receive a hex character
 */
uint8_t uartRecvHex(char ch) {
  if((ch>='0')&&(ch<='9'))
    return ch - '0';
  if((ch>='A')&&(ch<='F'))
    return ch - 'A' + 10;
  return 0xFF;
  }

/** Read a data line into the buffer.
 */
uint8_t uartRecvData() {
  char ch;
  uint8_t data, count = 0;
  uint16_t check = CHECKSUM_SEED;
  while(count<(BUFFER_SIZE * 2)) {
    ch = uartRecv();
    if(ch==EOL)
      break;
    data = uartRecvHex(ch);
    if(data==0xFF) {
      count = 0;
      break;
      }
    g_buffer[count / 2] = (g_buffer[count / 2] << 4) | data;
    count++;
    }
  count = (count + 1) / 2; // Round up to the number of full bytes
  // Consume until end of line
  while(ch!=EOL)
    ch = uartRecv();
  // Calculate checksum and verify it
  for(data=0; data<(count - CHECKSUM_SIZE); data++)
    check = checksum(check, g_buffer[data]);
  if(((check >> 8) & 0xFF)!=g_buffer[count - 2])
    return 0;
  if((check & 0xFF)!=g_buffer[count - 1])
    return 0;
  // Packet seems OK
  return count;
  }

/** Write a single character
 */
void uartSend(char ch) {
  }

/** Write the low nybble of the value in hex
 */
void uartSendHex(uint8_t value) {
  if(value < 10)
    uartSend('0' + value);
  else
    uartSend('A' + value - 10);
  }

/** Write the buffer as a data line
 */
void uartSendData(uint8_t size) {
  uint16_t check = CHECKSUM_SEED;
  for(uint8_t *pData = g_buffer; size; size--,pData++) {
    check = checksum(check, *pData);
    uartSendHex(((*pData)>>4)&0x0F);
    uartSendHex((*pData)&0x0F);
    }
  }

/** Program entry point
 */
void main() {
  if(!bootLoaderRequired())
    goto launch;
  // Set up the UART and start processing commands
  uartInit();
  while(true) {
    char ch = uartRecv();
    if(ch=='?') { // Get info
      if(uartRecv()!=EOL) {
        uartSend(FAIL);
        uartSend(EOL);
        }
      else {
        g_buffer[0] = PROTOCOL_VERSION;
        g_buffer[1] = CPU_TYPE;
        g_buffer[2] = CPU_MODEL;
        uartSend(OK);
        uartSendData(3);
        }
      }
    else if(ch=='!') { // Restart
      if(uartRecv()!=EOL) {
        uartSend(FAIL);
        uartSend(EOL);
        }
      else
        goto cleanup;
      }
    else if(ch=='W') { // Write data
      if(uartRecvData()!=WRITE_BYTES) {
        uartSend(FAIL);
        uartSend(EOL);
        }
      else {
        uartSend(OK);
        uartSend(EOL);
        }
      }
    else if(ch=='R') { // Read data
      if(uartRecvData()!=READ_BYTES) {
        uartSend(FAIL);
        uartSend(EOL);
        }
      else {
        uartSend(OK);
        uartSend(EOL);
        }
      }
    else { // Unknown character
      }
    }
cleanup: // Clean up any hardware
  uartDone();
launch: // Launch the main code
  ;
  }
