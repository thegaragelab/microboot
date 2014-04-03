#!/bin/sh
#-------------------------------------------------------------------
# Script to build all supported targets
#-------------------------------------------------------------------
TARGETS="attiny85 atmega8 atmega88 atmega168"

OPTIONS=
if [ "X$1" == "Xdebug" ]; then
  OPTIONS="SIMULATE=1"
fi

# Clean and build
rm -f *.hex
for target in ${TARGETS}; do
  echo "*** Building for ${target}"
  make ${OPTIONS} MCU=${target} clean; make ${OPTIONS} MCU=${target}
done
