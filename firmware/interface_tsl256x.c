#include "interface.h"
#include "usbtenki_cmds.h"
#include "i2c.h"
#include "tsl256x.h"

#include <util/delay.h>

/* USBTenki head PCB connects address select to GND */
#define TSL2561_ADDR    (TSL2561_ADDRESS_GND)

int sensors_init(void)
{
	unsigned char tmp;
	int res;
	
	i2c_init(I2C_FLAG_EXTERNAL_PULLUP, 55);

	/* Powerup the chip */
	tmp = 0x03;
	res = tsl2561_writeRegister(TSL2561_ADDR, 
				TSL2561_REG_CONTROL | TSL2561_CMD_CMD, &tmp, 1);
	if (res)
		return res;

	/* Setup gain and timing */
	tmp = 0x02;	// Integration time nominal 402ms
	res = tsl2561_writeRegister(TSL2561_ADDR, 
				TSL2561_REG_TIMING | TSL2561_CMD_CMD, &tmp, 1);
	if (res)
		return res;

	tmp = 0x00;	// No interrupts (0x00 is default value)
	res = tsl2561_writeRegister(TSL2561_ADDR, 
				TSL2561_REG_TIMING | TSL2561_CMD_CMD, &tmp, 1);
	if (res)
		return res;
	return 0;
}

static int switchGain(int gain_16x)
{
	static char last_16x = 0;
	unsigned char tmp;
	int res;

	if (last_16x != gain_16x) {
		tmp = 0x02;	// Integration time nominal 402ms
		if (gain_16x)
			tmp |= 0x10;	// 16x gain
		res = tsl2561_writeRegister(TSL2561_ADDR, 
				TSL2561_REG_TIMING | TSL2561_CMD_CMD, &tmp, 1);
		if (res)
			return res;
		
		_delay_ms(600);		
	}

	last_16x = gain_16x;

	return 0;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	int res;

	dst[0] = 0x00;
	dst[1] = 0x00;

	switch(id)
	{
		case 0:
			res = switchGain(0);
			res = tsl2561_readRegister(TSL2561_ADDR, 
					TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA0_L,
					dst, 2);
			break;
		case 1:
			res = switchGain(0);
			res = tsl2561_readRegister(TSL2561_ADDR, 
				 	TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA1_L,
					dst, 2);
			break;
		case 2:
			res = switchGain(1);
			res = tsl2561_readRegister(TSL2561_ADDR, 
					TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA0_L,
					dst, 2);
			break;
		case 3:
			res = switchGain(1);
			res = tsl2561_readRegister(TSL2561_ADDR, 
					TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA1_L,
					dst, 2);
			break;

	}

	return 2;
}


