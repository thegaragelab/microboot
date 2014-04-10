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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "hardware.h"
#include "microboot.h"

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

#if defined(__AVR_ATtiny85__)
#  define CPU_MODEL 0x01
#elif defined(__AVR_ATmega8__)
#  define CPU_MODEL 0x02
#elif defined(__AVR_ATmega88__)
#  define CPU_MODEL 0x03
#elif defined(__AVR_ATmega168__)
#  define CPU_MODEL 0x04
#else
#  error Unsupported CPU
#endif

uint8_t  g_buffer[BUFFER_SIZE];
uint8_t  g_pagecache[SPM_PAGESIZE];
uint16_t g_page_address;

//---------------------------------------------------------------------------
// Helper functions
//---------------------------------------------------------------------------

/** Declares the main program
 *
 * Simply defines a function pointer to the base of memory. Calling it will
 * execute the main program.
 */
void (*application)(void) = 0x0000;

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

//---------------------------------------------------------------------------
// Flash access
//---------------------------------------------------------------------------

// Check for SPM Control Register in processor.
#if defined (SPMCSR)
#  define __SPM_REG    SPMCSR
#elif defined (SPMCR)
#  define __SPM_REG    SPMCR
#else
#  error AVR processor does not provide bootloader support!
#endif

/** Write the current buffer contents into flash
 */
bool writeFlash() {
  uint16_t address = (((uint16_t)g_buffer[0] << 8) & 0xFF00) | g_buffer[1];
  uint8_t index, written = 0;
  while(written<DATA_SIZE) {
    g_page_address = (address / SPM_PAGESIZE) * SPM_PAGESIZE;
    // Read the page into the buffer
    for(index=0; index<SPM_PAGESIZE; index++)
      g_pagecache[index] = pgm_read_byte_near(address);
    // Add in our data
    uint8_t offset = (uint8_t)(address - g_page_address);
    for(index=0;(written<DATA_SIZE)&&((offset + index)<SPM_PAGESIZE);written++,index++)
      g_pagecache[offset + index] = g_buffer[written + 2];
    // Write the page
    asm volatile(
      // Y points to memory buffer, Z points to flash page
      "  lds   r30, g_page_address           \n\t"
      "  lds   r31, g_page_address + 1       \n\t"
      "  ldi   r28, lo8(g_pagecache)         \n\t"
      "  ldi   r29, hi8(g_pagecache)         \n\t"
      // First erase the selected page
      "  ldi   r16, (1<<PGERS) | (1<<SPMEN)  \n\t"
      "  rcall exec_spm                      \n\t"
#if !defined(__AVR_ATtiny85__)
      // Re-enable the RWW section
      "  ldi   r16, (1<<RWWSRE) | (1<<SPMEN) \n\t"
      "  rcall exec_spm"
#endif
      // Transfer data from RAM to Flash page buffer
      "  ldi   loopl, %[spm_pagesize]        \n\t"
      "write_loop:                           \n\t"
      "  ld    r0, Y+                        \n\t"
      "  ld    r1, Y+                        \n\t"
      "  ldi   r16, (1<<SPMEN)               \n\t"
      "  rcall exec_spm                      \n\t"
      "  adiw  ZH:ZL, 2                      \n\t"
      "  subi  looplo, 2                     \n\t"
      "  brne  write_loop                    \n\t"
      // Execute page write
      "  subi  ZL, %[spm_pagesize]           \n\t"
      "  ldi   r16, (1<<PGWRT) | (1<<SPMEN)  \n\t"
      "  rcall exec_spm                      \n\t"
#if !defined(__AVR_ATtiny85__)
      // Re-enable the RWW section
      "  ldi   r16, (1<<RWWSRE) | (1<<SPMEN) \n\t"
      "  rcall exec_spm                      \n\t"
#endif
      "  rjmp  end_write                     \n\t"
      "exec_spm:                             \n\t"
      // Wait for previous SPM to complete
      "wait_spm:                             \n\t"
      "  in    r17, %[spm_reg]               \n\t"
      "  sbrc  r17, SPMEN                    \n\t"
      "  rjmp  wait_spm                      \n\t"
      // SPM timed sequence
      "  out %[spm_reg], r16                 \n\t"
      "  spm                                 \n\t"
      "  ret                                 \n\t"
      "end_write:                            \n\t"
      : [spm_reg] "=m" (__SPM_REG)
      : [spm_pagesize] "M" (SPM_PAGESIZE)
      : "r0","r16","r17","r19","r28","r29","r30","r31");

// TODO: Write the page

    // Update addresses
    address = address + written;
    }
  return true;
  }

/** Fill the buffer from flash
 *
 * Uses the first two bytes of the global buffer as the address (high byte
 * followed by low byte) and fills the remainder of the buffer with DATA_SIZE
 * bytes from that address.
 */
bool readFlash() {
  uint16_t address = ((uint16_t)g_buffer[0] << 8) | g_buffer[1];
  for(uint8_t index = 0; index<DATA_SIZE; index++, address++)
    g_buffer[index + 2] = pgm_read_byte_near(address);
  return true;
  }

//---------------------------------------------------------------------------
// Serial interface
//---------------------------------------------------------------------------

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
  uartSendHex((check >> 12) & 0x0F);
  uartSendHex((check >> 8) & 0x0F);
  uartSendHex((check >> 4) & 0x0F);
  uartSendHex(check & 0x0F);
  }

/** Send a failure message
 */
inline void uartSendFail() {
  uartSend(FAIL);
  uartSend(EOL);
  }

//---------------------------------------------------------------------------
// Main program
//---------------------------------------------------------------------------

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
      if(uartRecv()!=EOL)
        uartSendFail();
      else {
        g_buffer[0] = PROTOCOL_VERSION;
        g_buffer[1] = DATA_SIZE;
        g_buffer[2] = CPU_TYPE;
        g_buffer[3] = CPU_MODEL;
        uartSend(OK);
        uartSendData(4);
        uartSend(EOL);
        }
      }
    else if(ch=='!') { // Restart
      if(uartRecv()!=EOL)
        uartSendFail();
      else
        goto cleanup;
      }
    else if(ch=='W') { // Write data
      if(uartRecvData()!=WRITE_BYTES)
        uartSendFail();
      else if(!writeFlash())
        uartSendFail();
      else {
        uartSend(OK);
        uartSend(EOL);
        }
      }
    else if(ch=='R') { // Read data
      if(uartRecvData()!=READ_BYTES)
        uartSendFail();
      else if(!readFlash())
        uartSendFail();
      else {
        uartSend(OK);
        uartSendData(ADDRESS_SIZE + DATA_SIZE);
        uartSend(EOL);
        }
      }
    else { // Unknown character
      while(ch!=EOL)
        ch = uartRecv();
      uartSendFail();
      }
    }
cleanup: // Clean up any hardware
  uartDone();
launch: // Launch the main code
  application();
  }
