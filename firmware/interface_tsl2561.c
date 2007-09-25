#include "interface.h"
#include "usbtenki_cmds.h"
#include "i2c.h"
#include "tsl2561.h"

/* USBTenki head PCB connects address select to GND */
#define TSL2561_ADDR    (TSL2561_ADDRESS_GND)

int sensors_init(void)
{
	unsigned char tmp;
	int res;
	
	i2c_init(I2C_FLAG_EXTERNAL_PULLUP);

	/* Powerup the chip */
	tmp = 0x03;
	res = tsl2561_writeRegister(TSL2561_ADDR, 
				TSL2561_REG_CONTROL | TSL2561_CMD_CMD, &tmp, 1);
	if (res)
		return res;

return 0;
	/* Setup gain and timing */
//	tmp = 0x01;	// Integration time nominal 101ms
	tmp = 0x02;	// Integration time nominal 402ms
//	tmp |= 0x80;	// 16x gain // TODO: Additional channels?
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

int sensors_getNumChannels(void)
{
	return 2;
}

int sensors_getChipID(unsigned char id)
{
	switch (id)
	{
		case 0:
			return USBTENKI_CHIP_TSL2561_IR_VISIBLE;
		case 1:
			return USBTENKI_CHIP_TSL2561_IR;
	}
	return USBTENKI_CHIP_NONE;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	dst[0] = 0x00;
	dst[1] = 0x00;

	switch(id)
	{
		case 0:
			tsl2561_readRegister(TSL2561_ADDR, 
					TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA0_L,
					dst, 2);
			break;
		case 1:
			tsl2561_readRegister(TSL2561_ADDR, 
					TSL2561_CMD_CMD | TSL2561_CMD_WORD | TSL2561_REG_DATA1_L,
					dst, 2);
			break;
	}

	return 2;
}


