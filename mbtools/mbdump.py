#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Utility to dump the contents of a device.
#----------------------------------------------------------------------------
import sys
from microboot import Microboot, MicrobootException

#--- Banner and usage information
BANNER = """
mbdump.py - Microboot/Microboard Firmware Dump Utility
Copyright (c) 2014, The Garage Lab. All Rights Reserved.
"""

#--- Communications log
g_logFile = None

#----------------------------------------------------------------------------
# Helper functions
#----------------------------------------------------------------------------

def showUsage():
  exit(1)

def logFunction(send, recv):
  g_logFile.write(">" + send)
  g_logFile.write("<" + recv)

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

if __name__ == "__main__":
  print BANNER.strip() + "\n"
  # Set up defaults
  device = None
  port = "/dev/ttyUSB0"
  filename = None
  # Process command line arguments
  index = 1
  while index < len(sys.argv):
    arg = sys.argv[index]
    if (arg == "--device") or (arg == "-d"):
      index = index + 1
      device = sys.argv[index]
    elif (arg == "--port") or (arg == "-p"):
      index = index + 1
      port = sys.argv[index]
    elif (arg == "--log"):
      g_logfile = open("transfer.log", "w")
    else:
      filename = arg
      index = index + 1
      if index <> len(sys.argv):
        print "Error: Filename must be the last parameter.\n"
        showUsage()
    index = index + 1
  # Rationalise parameters
  if device is None:
    print "Error: You must specify a device.\n"
    showUsage()
  if filename is None:
    filename = device + ".hex"
  # TODO: Add default extension to filename
  # Set up the device interface
  mb = Microboot()
  info = mb.getDeviceInfo(device)
  if info is None:
    print "Unsupported device type '%s'." % device
  # Show what we are doing
  print "Reading from '%s' on '%s'. Output in '%s'." % (device, port, filename)
  print "Will save %d bytes (0x%04X:0x%04X)." % (size, info[3], info[4])
  # Set up logging if requested
  if g_logFile is not None:
    mb.logger = logFunction
  # Connect to the device
  mb.connect(device, port)
  # Read everything
  size = info[4] - info[3] + 1
  data = mb.read(info[3], size)
  mb.disconnect()
  # TODO: Save the data
  print data

