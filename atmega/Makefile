#----------------------------------------------------------------------------
# Makefile for the AVR Microboot firmware
#----------------------------------------------------------------------------

# CPU specific options
MCU       = atmega8
F_CPU     = 8000000
FUSE_LOW  = 0xE2
FUSE_HIGH = 0xDF
FUSE_EXT  = 0xFF

# Define tools and options
TARGET  := microboot
SIZE    := avr-size
CXX     := avr-gcc
CFLAGS  := -std=gnu99 -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -ffreestanding
LDFLAGS := -mmcu=$(MCU)
OBJCOPY := avr-objcopy
AVRDUDE := avrdude

# Main source
SOURCES = \
  microboot.c

ifeq ($(MCU), attiny85)
  SOURCES += uart_tiny.c
else
  SOURCES += uart_mega.c
endif

# Derive object file names
OBJECTS = $(filter-out $(SOURCES), $(patsubst %.c,%.o,$(SOURCES)) $(patsubst %.S,%.o,$(SOURCES)))

.PHONY: all clean

all: $(TARGET).hex
	@cp $(TARGET).hex $(TARGET)-$(MCU).hex

clean:
	@rm -f $(OBJECTS) $(TARGET).hex $(TARGET).elf

flash: $(TARGET).hex
	@$(AVRDUDE) -c usbasp -p t85 -U flash:w:$(TARGET).hex -u

fuses:
	@$(AVRDUDE) -c usbasp -p t85 -u -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXT):m

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