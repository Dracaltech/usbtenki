#include "interface.h"
#include "usbtenki_cmds.h"
#include "sensirion_serial.h"

int sensors_init(void)
{
	sser_init();
	return 0;	
}

int sensors_getNumChannels(void)
{
	return 2;
}

int sensors_getChipID(unsigned char id)
{
	switch (id)
	{
		case 0: return USBTENKI_CHIP_SHT_TEMP;
		case 1: return USBTENKI_CHIP_SHT_RH;
	}
	return -1;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	int res;

	switch (id)
	{
		case 0:
			res = sser_getWord(SHT_CMD_MEASURE_TEMPERATURE, dst);
			if (res)
				return res;
			break;

		case 1:
			res = sser_getWord(SHT_CMD_MEASURE_HUMIDITY, dst);
			if (res)
				return res;			
			break;

		default:
			return -1;
	}
	return 2;
}


