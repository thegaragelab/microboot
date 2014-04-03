#!/bin/sh
#----------------------------------------------------------------------------
# 03-Apr-2014 Shane G
#
# Simulate the bootloader with 'simulavr'. Requires the older (1.0.0) version
# for interactive sessions.
#----------------------------------------------------------------------------

# Check parameters
if [ "X$1" == "X" ]; then
  echo "Error: Please specify the target processor."
  exit 1
fi

# Make sure the .elf file is available
if [ ! -f microboot-$1.elf ]; then
  echo "Error: Could not find microboot-$1.elf"
  exit 1
fi

# Now run the debugging session
simulavr -W 0x20,- -R 0x22,- -d $1 --file microboot-$1.elf

