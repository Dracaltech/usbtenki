# Name: Makefile
# Project: PowerSwitch
# Author: Christian Starkjohann
# Creation Date: 2004-12-29
# Tabsize: 4
# Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
# License: Proprietary, free under certain conditions. See Documentation.
# This Revision: $Id: Makefile,v 1.10 2007-05-26 01:13:07 raph Exp $

SERIAL = /dev/avr
UISP = uisp -dprog=stk500 -dserial=$(SERIAL) -dpart=ATmega8
# The two lines above are for "uisp" and the AVR910 serial programmer connected
# to a Keyspan USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

SERNUMBER=

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=atmega8 -I../common -DF_CPU=12000000UL

COMMON_OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o serno.o i2c.o main.o eeprom.o adc.o
SENSOR_OBJECTS = mcp9800.o lm75.o sensirion_serial.o
# Note that we link usbdrv.o first! This is required for correct alignment of
# driver-internal global variables!

# symbolic targets:
all: mcp9800.hex lm75.hex sensirion.hex

%.o: %.c
	$(COMPILE) -c $< -o $@

%.o: %.c %.h
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
	

#
# Fuse high byte = 0xC1
#  | RSTDISBL | WDTON | SPIEN | CKOPT | EESAVE | BOOTSZ1-0 | BOOTRST |
#  |    1         1       0       0       0       0    0        1
#
# EESAVE: Eeprom not erased when doing a chip erase (flashing)!
#
# Fuse low byte = 0x1f
#  | BODLEVEL | BODEN | SUT1 | SUT0 | CKSEL3   2   1   0   |
#  |    0         0      0       1      1      1   1   1   |
#       
# BODLEVEL: Brownout at 4 volts.
#
fuse:
	$(UISP) --wr_fuse_h=0xc1 --wr_fuse_l=0x1f

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
