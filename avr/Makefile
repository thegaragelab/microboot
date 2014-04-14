#----------------------------------------------------------------------------
# Makefile for the AVR Microboot firmware
#----------------------------------------------------------------------------

# Debugging options
SIMULATE  = 0

# CPU specific options
MCU       = atmega8
F_CPU     = 8000000
FUSE_LOW  = 0xE2
FUSE_HIGH = 0xDF
FUSE_EXT  = 0xFF

# Define tools and options
TARGET    := microboot
SIZE      := avr-size
CXX       := avr-gcc
CFLAGS    := -std=gnu99 -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -ffreestanding
LDFLAGS   := -mmcu=$(MCU) -Wl,-Map,$(TARGET)-$(MCU).map
OBJCOPY   := avr-objcopy
AVRDUDE   := avrdude
PROCESSOR :=

# Fuse settings and start address
ifeq ($(MCU), attiny85)
  PROCESSOR = t85
  FUSE_HIGH  = 0x5D
#  FUSE_HIGH  = 0xDD (RESET enabled)
  FUSE_LOW   = 0xE2
  FUSE_EXT   = 0xFE
  LDFLAGS   += -Wl,--relax,--gc-sections -Wl,--section-start=.text=0x1c00
endif
ifeq ($(MCU), atmega8)
  PROCESSOR = m8
#  FUSE_HIGH  = 0x5A (RESET disabled)
#  FUSE_HIGH  = 0xDA (RESET enabled)
  FUSE_HIGH  = 0xD9
  FUSE_LOW   = 0xE4
  BOOT_START = 0x1C00
endif
ifeq ($(MCU), atmega88)
  PROCESSOR = m88
  FUSE_HIGH  = 0xDF
#  FUSE_HIGH  = 0x5F (RESET disabled)
  FUSE_LOW   = 0xE2
  FUSE_EXT   = 0xFA
  BOOT_START = 0x1C00
endif
ifeq ($(MCU), atmega168)
  PROCESSOR = m168
  FUSE_HIGH  = 0xDF
#  FUSE_HIGH  = 0x5F (RESET disabled)
  FUSE_LOW   = 0xE2
  FUSE_EXT   = 0xFA
  BOOT_START = 0x3C00
endif

# Verify that the MCU is supported
ifeq ($(PROCESSOR),)
  $(error Unsupported AVR specified - $(MCU))
endif

# Main source
SOURCES = \
  microboot.c

# Add the appropriate UART implementation source
ifneq ($(SIMULATE), 0)
  SOURCES += uart_simulavr.c
else
  ifeq ($(MCU), attiny85)
    SOURCES += uart_tiny.c
  else
    SOURCES += uart_mega.c
  endif
endif

# Derive object file names
OBJECTS = $(filter-out $(SOURCES), $(patsubst %.c,%.o,$(SOURCES)) $(patsubst %.S,%.o,$(SOURCES)))

.PHONY: all clean

all: $(TARGET)-$(MCU).hex $(TARGET)-$(MCU).elf\

$(TARGET)-$(MCU).elf: $(TARGET).elf
	@mv $(TARGET).elf $(TARGET)-$(MCU).elf

$(TARGET)-$(MCU).hex: $(TARGET).hex
	@mv $(TARGET).hex $(TARGET)-$(MCU).hex

clean:
	@rm -f $(OBJECTS) $(TARGET).hex $(TARGET).elf

flash:
	$(AVRDUDE) -c usbasp -p $(PROCESSOR) -U flash:w:$(TARGET)-$(MCU).hex -u

fuses:
ifeq ($(MCU), atmega8)
	$(AVRDUDE) -c usbasp -p $(PROCESSOR) -u -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m
else
	$(AVRDUDE) -c usbasp -p $(PROCESSOR) -u -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXT):m
endif

$(TARGET).elf: $(OBJECTS)
	@echo Linking
	@$(CXX) $(LDFLAGS) -o $@ $(OBJECTS)
	@$(SIZE) --mcu=$(MCU) --format=avr $@

$(TARGET).hex: $(TARGET).elf
	@echo Creating Hex File
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

# How to compile
%.o: %.c
	@echo Compiling $<
	@$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo Assembling $<
	@$(CXX) $(CFLAGS) -c $< -o $@