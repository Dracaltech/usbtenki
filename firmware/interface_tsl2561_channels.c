#include "interface.h"
#include "usbtenki_cmds.h"


int sensors_getNumChannels(void)
{
	return 4;
}

int sensors_getChipID(unsigned char id)
{
	switch (id)
	{
		case 0:
			return USBTENKI_CHIP_TSL2561_IR_VISIBLE;
		case 1:
			return USBTENKI_CHIP_TSL2561_IR;
		case 2:
			return USBTENKI_CHIP_TSL2561_IR_VISIBLE_16X;
		case 3:
			return USBTENKI_CHIP_TSL2561_IR_16X;
	}
	return USBTENKI_CHIP_NONE;
}

