#include "interface.h"
#include "usbtenki_cmds.h"
#include "i2c.h"
#include "lm75.h"

#define LM75_ADDR       (LM75_ADDR_BASE + 7)

int sensors_init(void)
{
	i2c_init(I2C_FLAG_INTERNAL_PULLUP);

	return lm75_configure(LM75_ADDR, 0);
}

int sensors_getNumChannels(void)
{
	return 1;
}

int sensors_getChipID(unsigned char id)
{
	return USBTENKI_CHIP_LM75;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	int res;
	res = lm75_readRegister(LM75_ADDR, LM75_REG_TEMP, dst, 2);
	if (res)
		return res;
	return 2;
}

