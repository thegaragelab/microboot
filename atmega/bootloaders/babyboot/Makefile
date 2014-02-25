# Makefile for ATmegaBOOT
# E.Lins, 2004-10-14

# program name should not be changed...
PROGRAM = avrboot

OPTIMIZE    = -Os
#OPTIMIZE   = -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-split-wide-types -Wl,--relax -fno-inline-small-functions -mcall-prologues
#OPTIMIZE   = -Os -fdata-sections -fpack-struct -Wl,--relax  -mcall-prologues

CC         = avr-gcc

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -D$(MCU)
override LDFLAGS       = -Wl,-Map,$(TARGET).map,$(LDSECTION)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size

all: microboard_8

clean:
	rm -rf *.s
	rm -rf *.o *.elf
	rm -rf *.lst *.map

#----------------------------------------------------------------------------
# ATmega Microboard
#----------------------------------------------------------------------------

# Microboard - ATmega8 @8MHz
#
microboard_8: TARGET = microboard_8
microboard_8: MCU = atmega8
microboard_8: MCU_TARGET = atmega8
microboard_8: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DMICROBOARD
microboard_8: LDSECTION  = --section-start=.text=0x1800
microboard_8: $(PROGRAM).o
microboard_8: microboard_8.elf
microboard_8: microboard_8.hex

# Microboard - ATmega88 @8MHz
#
microboard_88: TARGET = microboard_88
microboard_88: MCU = atmega88
microboard_88: MCU_TARGET = atmega88
microboard_88: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DMICROBOARD
microboard_88: LDSECTION  = --section-start=.text=0x1c00
microboard_88: $(PROGRAM).o
microboard_88: microboard_88.elf
microboard_88: microboard_88.hex

# Microboard - ATmega168 @8MHz
#
microboard_168: TARGET = microboard_168
microboard_168: MCU = atmega168
microboard_168: MCU_TARGET = atmega168
microboard_168: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DMICROBOARD
microboard_168: LDSECTION  = --section-start=.text=0x3800
microboard_168: $(PROGRAM).o
microboard_168: microboard_168.elf
microboard_168: microboard_168.hex

# Common rules
#
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	$(SIZE) $@
	md5sum $@

%.elf:
	mv $(PROGRAM).o $(TARGET).o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TARGET).o $(LIBS)

$(PROGRAM).o: $(PROGRAM).c

