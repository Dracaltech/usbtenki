# Name: Makefile
# Project: PowerSwitch
# Author: Christian Starkjohann
# Creation Date: 2004-12-29
# Tabsize: 4
# Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
# License: Proprietary, free under certain conditions. See Documentation.
# This Revision: $Id: Makefile,v 1.1 2007-04-27 02:20:37 raph Exp $

SERIAL = /dev/avr
UISP = uisp -dprog=stk500 -dserial=$(SERIAL) -dpart=ATmega8
# The two lines above are for "uisp" and the AVR910 serial programmer connected
# to a Keyspan USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=atmega8 #-DDEBUG_LEVEL=2
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o mcp9800.o i2c.o
# Note that we link usbdrv.o first! This is required for correct alignment of
# driver-internal global variables!


# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	#$(UISP) --erase --upload --verify if=main.hex
	$(UISP) --erase --upload if=main.hex

fuse:
	$(UISP) --wr_fuse_h=0xc9 --wr_fuse_l=0x9f

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	./checksize main.bin
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
