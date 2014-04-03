#!/bin/sh
#-------------------------------------------------------------------
# Script to build all supported targets
#-------------------------------------------------------------------
TARGETS="attiny85 atmega8 atmega88 atmega168"

rm -f *.hex
for target in ${TARGETS}; do
  echo "*** Building for ${target}"
  make clean; make MCU=${target}
done
