#include "interface.h"
#include "../common/usbtenki_cmds.h"

int sensors_init(void)
{
	return 0;
}

int sensors_getNumChannels(void)
{
	return 0;
}

int sensors_getChipID(unsigned char id)
{
	return USBTENKI_CHIP_NONE;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	return -1;
}
