#include "interface.h"
#include "usbtenki_cmds.h"
#include "i2c.h"
#include "se95.h"
#include "mcp9800.h" // For register access funcs.

static unsigned char se95_addr;

int sensors_init(void)
{
	unsigned char tmp[2];

	i2c_init(I2C_FLAG_INTERNAL_PULLUP, 255);

	for (se95_addr = SE95_ADDR_BASE; se95_addr <= SE95_ADDR_BASE+7; se95_addr++)
	{
		usbtenki_delay_ms(10);
		if (0 == mcp9800_readRegister(se95_addr, SE95_REG_TEMP, tmp, 2)) {
			return 0;
		}
	}

	return -1;
}

int sensors_getNumChannels(void)
{
	return 1;
}

int sensors_getChipID(unsigned char id)
{
	return USBTENKI_CHIP_SE95;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	int res;
	res = mcp9800_readRegister(se95_addr, SE95_REG_TEMP, dst, 2);
	if (res)
		return res;
	return 2;
}


