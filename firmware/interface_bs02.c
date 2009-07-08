#include "interface.h"
#include "usbtenki_cmds.h"
#include "sser.h"
#include <util/delay.h>

int sensors_init(void)
{
	// SHT7x datasheet specifies 11ms. With 50ms, we are on the safe side.
	_delay_ms(50);

	sser_init();

	// Explicitely set the status register with default values.
	// 
	// But the BS02 sensor seems to ignore the 14/12bit temp and 12/8bit hum.
	// setting and always returns data in 12 and 8 bits anyway...
	sser_cmd(SHT_CMD_WRITE_STATUS);
	sser_writeByte(0x01);

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
		case 0: return USBTENKI_CHIP_BS02_TEMP;
		case 1: return USBTENKI_CHIP_BS02_RH;
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

