#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Utility to flash a microcontroller over serial connections.
#----------------------------------------------------------------------------
import sys
from microboot import Microboot, MicrobootException

#--- Banner and usage information
BANNER = """
mbflash.py - Microboot/Microboard System Flashing Utility
Copyright (c) 2014, The Garage Lab. All Rights Reserved.
"""

#----------------------------------------------------------------------------
# Helper functions
#----------------------------------------------------------------------------
def showUsage():
  exit(1)

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

if __name__ == "__main__":
  print BANNER.strip()
