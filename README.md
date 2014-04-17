# Microboot - A Simple Bootloader Implementation

This is the source for a simple bootloader for a range of microcontroller units.
Originally developed for the [Microboard prototyping system](https://github.com/thegaragelab/microboard)
it has now been spun off into it's own project.

The source code presented here includes implementations of the bootloader for
a range of processors and a set of Python utilities to read and write the flash.
It is released under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/)
which allows you to use and modify the sources for your own needs.

Details of the operation and implementation of this bootloader can be found on
a series of posts on [The Garage Lab](http://thegaragelab.com) website. These
are:

* [Microboot - A Simple Bootloader](http://thegaragelab.com/posts/microboot-a-simple-bootloader.html)
* [Microboot on the ATtiny85](http://thegaragelab.com/posts/microboot-on-the-attiny85)

# Version History

The 'master' branch will always contain the source for the latest stable
release. The versions currently available are:

## V1.0.0

Initial release. The only MCU supported by this release is the Atmel ATtiny85.

# Acknowledgements

This software includes a number of components developed by others.

## The '*intelhex*' Library

Reading and writing of HEX format data files is implemented using the
['intelhex' library](http://www.bialix.com/intelhex/) developed by Alexander
Belchenko and released under a [BSD license](http://www.bialix.com/intelhex/LICENSE.txt).

## One Pin Software UART

The single pin UART design and code came from [this site](http://nerdralph.blogspot.com.au/2014/01/avr-half-duplex-software-uart.html)
and was developed by [Ralph Doncaster](https://www.blogger.com/profile/00037504544742962130).

