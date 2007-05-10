# Name: Makefile
# Project: PowerSwitch
# Author: Christian Starkjohann
# Creation Date: 2004-12-29
# Tabsize: 4
# Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
# License: Proprietary, free under certain conditions. See Documentation.
# This Revision: $Id: Makefile,v 1.7 2007-05-10 23:21:07 raph Exp $

SERIAL = /dev/avr
UISP = uisp -dprog=stk500 -dserial=$(SERIAL) -dpart=ATmega8
# The two lines above are for "uisp" and the AVR910 serial programmer connected
# to a Keyspan USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

SERNUMBER=

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=atmega8 -I../common \
		-DSERIAL_NUMBER=\'B\',\'0\',\'0\',\'0\',\'0\',\'1\' #-DDEBUG_LEVEL=2
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

COMMON_OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o serno.o i2c.o main.o 
SENSOR_OBJECTS = mcp9800.o lm75.o sensirion_serial.o
# Note that we link usbdrv.o first! This is required for correct alignment of
# driver-internal global variables!

# symbolic targets:
all: mcp9800.hex lm75.hex sensirion.hex

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

flash:
	echo "flash_sensirion | flash_lm75 | flash_mcp9800 ?"

flash_sensirion:	sensirion.hex
	$(UISP) --erase --upload if=sensirion.hex

flash_mcp9800:	mcp9800.hex
	$(UISP) --erase --upload if=mcp9800.hex

flash_lm75: lm75.hex
	$(UISP) --erase --upload if=lm75.hex
	

fuse:
	$(UISP) --wr_fuse_h=0xc9 --wr_fuse_l=0x9f

clean:
	rm -f mcp9800.hex mcp9800.lst mcp9800.obj mcp9800.cof mcp9800.list mcp9800.map mcp9800.eep.hex mcp9800.elf *.o usbdrv/*.o mcp9800.s usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
mcp9800.elf: $(COMMON_OBJECTS) mcp9800.o interface_mcp9800.o
	$(COMPILE) -o $@ $^

lm75.elf: $(COMMON_OBJECTS) lm75.o interface_lm75.o
	$(COMPILE) -o $@ $^

sensirion.elf: $(COMMON_OBJECTS) sensirion_serial.o interface_sensirion.o
	$(COMPILE) -o $@ $^


%.hex: %.elf
	rm -f $@
	avr-objcopy -j .text -j .data -O ihex $< $@
	./checksize $<

#main.hex:	main.elf
#	rm -f main.hex main.eep.hex
#	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
#	./checksize main.elf
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

#cpp:
#	$(COMPILE) -E main.c
