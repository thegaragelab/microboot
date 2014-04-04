#!/usr/bin/env python
#----------------------------------------------------------------------------
# 04-Mar-2014 ShaneG
#
# Support library for the Microboot protocol
#----------------------------------------------------------------------------
import collections

# We need PySerial
try:
  import serial
except:
  print "Error: This tool requires the pySerial module. Please install it."
  exit(1)

#--- Supported chips
CHIPLIST = {
  # Chip         Type  Model Protocol AddrL   AddrH
  "attiny85":  ( 0x01, 0x01, 0x10,    0x0000, 0xFFFF ),
  "atmega8":   ( 0x01, 0x02, 0x10,    0x0000, 0xFFFF ),
  "atmega88":  ( 0x01, 0x03, 0x10,    0x0000, 0xFFFF ),
  "atmega168": ( 0x01, 0x04, 0x10,    0x0000, 0xFFFF ),
  }

#--- Protocol constants
CMD_READ      = 'R'    # Character for the READ command
CMD_WRITE     = 'W'    # Character for the WRITE command
CMD_QUERY     = '?'    # Character for the QUERY command
CMD_RESET     = '!'    # Character for the RESET command
EOL           = '\n'   # Character to use for line termination
DATA_SIZE     = 16     # Number of data bytes in a packet
CHECKSUM_SEED = 0x5050 # Seed value for calculating checksum

#----------------------------------------------------------------------------
# Public API and classes
#----------------------------------------------------------------------------

class MicrobootException(Exception):
  """ Used to report protocol and connection related errors.
  """

  def __init__(self, message):
    """ Constructor with a descriptive message

        @param message the descriptive message for the exception.
    """
    self.message = message

  def __str__(self):
    """ Represent this exception as a string

        @return a string representation of the exception
    """
    return "Microboot Exception: " + self.message

class Microboot:
  """ Main helper class for the Microboot flash loader client
  """

  def __init__(self):
    pass

  #--------------------------------------------------------------------------
  # Helper methods
  #--------------------------------------------------------------------------

  def makeArray(self, data):
    """ Ensure the data parameter is actually a sequence of integers

        @param data the data value to convert to a sequence

        @return the parameter as a sequence.
    """
    if not isinstance(data, collections.Sequence):
      if typeof(data) == str:
        data = [ chr(x) for x in data ]
      else:
        data = ( data, )
    return data

  def checksum(self, total, data, offset = 0, length = -1):
    """ Add the given data to the checksum

        @param total the current checksum total
        @param data a sequence of bytes or a single integer value.
        @param offset the offset into the data array to start adding
        @param length the number of bytes to add (-1 for all)

        @return the new checksum

        @throws MicrobootException if the array does not contain enough data
                for the parameters provided or one of the values in the array
                is out of range.
    """
    # Make sure we have a sequence
    data = self.makeArray(data)
    # Adjust the length if required
    if length < 0:
      length = len(data) - offset
    # Make sure there is enough data
    if offset > len(data):
      raise MicrobootException("Data array is not of sufficient size (%d > %d)." % (offset, len(data)))
    if (length + offset) > len(data):
      raise MicrobootException("Data array is not of sufficient size (%d > %d)." % (length + offset, len(data)))
    # Now update the checksum
    for val in data:
      val = int(val)
      if (val < 0) or (val > 255):
        raise MicrobootException("Byte value is out of range (%d)." % val)
      total = (total + val) & 0xFFFF
    # Done
    return total

  def createWriteCommand(self, address, data, offset = 0):
    """ Create the 'WRITE' command string
    """
    # Make sure we have a sequence
    data = self.makeArray(data)
    # Calculate the checksum first
    check = CHECKSUM_SEED
    check = self.checksum(check, ((address >> 8) & 0xFF, address & 0xFF))
    check = self.checksum(check, data, offset, DATA_SIZE)
    # Now build up the string and return it
    return "%c%02X%02X%s%02X%02X%c" % (
      CMD_WRITE,
      (address >> 8) & 0xFF, # Address high byte
      address & 0xFF, # Address low byte
      "".join([ "%02X" % val for val in data[offset:offset + DATA_SIZE] ]),
      (check >> 8) & 0xFF, # Checksum high byte
      check & 0xFF, # Checksum low byte
      EOL
      )

  #--------------------------------------------------------------------------
  # Public methods
  #--------------------------------------------------------------------------

  def connect(self, port, device):
    """ Connect to the device attached to the given port.

        This will fail if there is a communication error on the port (or the
        port does not exist), the device is not supported or the device on the
        other end of the connection is not the expected device.

        @param port the name of the serial port to connect on.
        @param device the name of the device to expect.

        @return the device information tuple (@see getInfo)

        @throws MicrobootException if the connection attempt failed.
    """
    pass

  def connected(self):
    """ Determine if we are connected to a device

        @return True if a connection is currently active.
    """
    return False

  def disconnect(self):
    """ Disconnect from the device.

        Close the connection to the device (and the underlying serial port). If
        the connection is not established yet this method will have no effect.
    """
    pass

  def getInfo(self):
    """ Get the device information tuple.

        This method queries the device for it's information block which includes
        processor family, processor type and protocol version.

        @return a 3-tuple of integers consisting of family, model and protocol
                or None if a connection has not been established.
    """
    return None

  def read(self, start, length):
    """ Read data from the flash memory

        Query the chip for the contents of flash starting at 'start' for 'length'
        bytes.

        @param start the start address to read from. Must be a positive integer
                     in the allowable address range for the device.
        @param length the number of bytes to read. Must be a positive integer
                      greater than zero such that start + length is in the valid
                      address range for the device.

        @return an array of bytes representing the contents of the flash memory
                or None if the device is not connected.

        @throws MicrobootException if the address is out of range or a
                communication error occurs.
    """
    return None

  def write(self, start, length, data):
    """ Write data to the flash memory

        Replace the contents of the flash memory with the given bytes starting
        at address 'start' for 'length' bytes.

        @param start the start address to read from. Must be a positive integer
                     in the allowable address range for the device.
        @param length the number of bytes to read. Must be a positive integer
                      greater than zero such that start + length is in the valid
                      address range for the device.
        @param data an array of bytes to be written to the flash. The length of
                    the must be at least 'length' bytes.

        @throws MicrobootException if the address is out of range or a
                communication error occurs.
    """
    pass

  def reset(self):
    """ Reset the device.

        Asks the device to reset (start the application program).
    """
    pass

# Make sure we are just being imported
if __name__ == "__main__":
  #print "Error: This file is a support library, do not run it directly."
  #exit(1)
  mb = Microboot()
  print mb.createWriteCommand(0x1000, (0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f))

