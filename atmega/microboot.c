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
#define CPU_MODEL        0x02

// Size of flash pages in memory
#define PAGE_SIZE	 0x20

uint8_t g_buffer[BUFFER_SIZE];

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

/** Write the current buffer contents into flash
 */
bool writeFlash() {
  while(bit_is_set(EECR,EEWE)); // Wait for previous EEPROM writes to complete
  asm volatile(
    "  clr        r17                   \n\t"   //page_word_count
    "  lds        r30,g_buffer          \n\t"   //Address of FLASH location (in bytes)
    "  lds        r31,g_buffer+1        \n\t"
    "  ldi        r28,lo8(g_buffer + 2) \n\t"   //Start of buffer array in RAM
    "  ldi        r29,hi8(g_buffer + 2) \n\t"
    "  ldi        r24,0                 \n\t"     //Length of data to be written (in bytes)
    "  ldi        r25,%[data_size]      \n\t"
    "  rjmp       length_loop           \n\t"
    "  adiw       r24,1                 \n\t"
    "length_loop:                       \n\t"   //Main loop, repeat for number of words in block
    "  cpi        r17,0x00              \n\t"   //If page_word_count=0 then erase page
    "  brne       no_page_erase         \n\t"
    "  rcall      wait_spm              \n\t"
    "  ldi        r16,0x03              \n\t"   //Erase page pointed to by Z
    "  sts        %0,r16                \n\t"
    "  spm                              \n\t"
    "  rcall      wait_spm              \n\t"
    "  ldi        r16,0x11              \n\t"   //Re-enable RWW section
    "  sts        %0,r16                \n\t"
    "  spm                              \n\t"
    "no_page_erase:                     \n\t"
    "  ld         r0,Y+                 \n\t"   //Write 2 bytes into page buffer
    "  ld         r1,Y+                 \n\t"
    "  rcall      wait_spm              \n\t"
    "  ldi        r16,0x01              \n\t"   //Load r0,r1 into FLASH page buffer
    "  sts        %0,r16                \n\t"
    "  spm                              \n\t"
    "  inc        r17                   \n\t"   //page_word_count++
    "  cpi        r17,%1                \n\t"
    "  brlo       same_page             \n\t"   //Still same page in FLASH
    "write_page:                        \n\t"
    "  clr        r17                   \n\t"   //New page, write current one first
    "  rcall      wait_spm              \n\t"
    "  ldi        r16,0x05              \n\t"   //Write page pointed to by Z
    "  sts        %0,r16                \n\t"
    "  spm                              \n\t"
    "  rcall      wait_spm              \n\t"
    "  ldi        r16,0x11              \n\t"   //Re-enable RWW section
    "  sts        %0,r16                \n\t"
    "  spm                              \n\t"
    "same_page:                         \n\t"
    "  adiw       r30,2                 \n\t"   //Next word in FLASH
    "  sbiw       r24,2                 \n\t"   //length-2
    "  breq       final_write           \n\t"   //Finished
    "  rjmp       length_loop           \n\t"
    "wait_spm:                          \n\t"
    "  lds        r16,%0                \n\t"   //Wait for previous spm to complete
    "  andi       r16,1                 \n\t"
    "  cpi        r16,1                 \n\t"
    "  breq       wait_spm              \n\t"
    "  ret                              \n\t"
    "final_write:                       \n\t"
    "  cpi        r17,0                 \n\t"
    "  breq       block_done            \n\t"
    "  adiw       r24,2                 \n\t"   //length+2, fool above check on length after short page write
    "  rjmp       write_page            \n\t"
    "block_done:                        \n\t"
    "  clr        __zero_reg__          \n\t"   //restore zero register
    : "=m" (SPMCR)
    : "M" (PAGE_SIZE), [data_size] "I" (DATA_SIZE)
    : "r0","r16","r17","r24","r25","r28","r29","r30","r31");
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
        g_buffer[1] = CPU_TYPE;
        g_buffer[2] = CPU_MODEL;
        uartSend(OK);
        uartSendData(3);
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
