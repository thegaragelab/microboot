# Makefile for ATmegaBOOT
# E.Lins, 2004-10-14

# program name should not be changed...
PROGRAM1    = ATmegaBOOT
PROGRAM2    = optiboot

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

all: babyduino8_nc

clean:
	rm -rf *.s
	rm -rf *.o *.elf
	rm -rf *.lst *.map

#----------------------------------------------------------------------------
# Babyduino (Mk 2)
#----------------------------------------------------------------------------

# Babyduino - ATmega8 @8MHz
#
babyduino2_8: TARGET = babyduino2_8
babyduino2_8: MCU = atmega8
babyduino2_8: MCU_TARGET = atmega8
babyduino2_8: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DBABYDUINO2
babyduino2_8: LDSECTION  = --section-start=.text=0x1c00
babyduino2_8: PROGRAM = $(PROGRAM1)
babyduino2_8: $(PROGRAM1).o
babyduino2_8: babyduino2_8.elf
babyduino2_8: babyduino2_8.hex

# Babyduino - ATmega88 @8MHz
#
babyduino2_88: TARGET = babyduino2_88
babyduino2_88: MCU = atmega88
babyduino2_88: MCU_TARGET = atmega88
babyduino2_88: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DBABYDUINO2
babyduino2_88: LDSECTION  = --section-start=.text=0x1c00
babyduino2_88: PROGRAM = $(PROGRAM2)
babyduino2_88: $(PROGRAM2).o
babyduino2_88: babyduino2_88.elf
babyduino2_88: babyduino2_88.hex

# Babyduino - ATmega168 @8MHz
#
babyduino2_168: TARGET = babyduino2_168
babyduino2_168: MCU = atmega168
babyduino2_168: MCU_TARGET = atmega168
babyduino2_168: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400 -DBABYDUINO2
babyduino2_168: LDSECTION  = --section-start=.text=0x3c00
babyduino2_168: PROGRAM = $(PROGRAM2)
babyduino2_168: $(PROGRAM2).o
babyduino2_168: babyduino2_168.elf
babyduino2_168: babyduino2_168.hex

#----------------------------------------------------------------------------
# Babyduino (Original)
#----------------------------------------------------------------------------

# Babyduino - ATmega8/Internal clock @8MHz
#
babyduino8_nc: TARGET = babyduino8_nc
babyduino8_nc: MCU = atmega8
babyduino8_nc: MCU_TARGET = atmega8
babyduino8_nc: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400
babyduino8_nc: LDSECTION  = --section-start=.text=0x1c00
babyduino8_nc: PROGRAM = $(PROGRAM1)
babyduino8_nc: $(PROGRAM1).o
babyduino8_nc: babyduino8_nc.elf
babyduino8_nc: babyduino8_nc.hex

# Babyduino - ATmega88/Internal clock @8MHz
#
babyduino88_nc: TARGET = babyduino88_nc
babyduino88_nc: MCU = atmega88
babyduino88_nc: MCU_TARGET = atmega88
babyduino88_nc: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400
babyduino88_nc: LDSECTION  = --section-start=.text=0x1c00
babyduino88_nc: PROGRAM = $(PROGRAM2)
babyduino88_nc: $(PROGRAM2).o
babyduino88_nc: babyduino88_nc.elf
babyduino88_nc: babyduino88_nc.hex

# Babyduino - ATmega168/Internal clock @8MHz
#
babyduino168_nc: TARGET = babyduino168_nc
babyduino168_nc: MCU = atmega168
babyduino168_nc: MCU_TARGET = atmega168
babyduino168_nc: CFLAGS += -DF_CPU=8000000 -DBAUD_RATE=38400
babyduino168_nc: LDSECTION  = --section-start=.text=0x3c00
babyduino168_nc: PROGRAM = $(PROGRAM2)
babyduino168_nc: $(PROGRAM2).o
babyduino168_nc: babyduino168_nc.elf
babyduino168_nc: babyduino168_nc.hex

# Common rules
#
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	$(SIZE) $@
	md5sum $@

%.elf:
	cp $(PROGRAM).o $(TARGET).o
	rm $(PROGRAM).o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TARGET).o $(LIBS)

$(PROGRAM1).o: $(PROGRAM1).c
$(PROGRAM2).o: $(PROGRAM2).c

