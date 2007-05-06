#include "interface.h"
#include "usbtemp_cmds.h"
#include "i2c.h"
#include "mcp9800.h"

#define MCP9800_ADDR    (MCP9800_ADDR_BASE + 7)

int sensors_init(void)
{
	i2c_init();

	return mcp9800_configure(MCP9800_ADDR, MCP9800_CFG_12BITS);
}

int sensors_getNumChannels(void)
{
	return 1;
}

int sensors_getChipID(unsigned char id)
{
	return USBTEMP_CHIP_MCP9800;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	int res;
	res = mcp9800_readRegister(MCP9800_ADDR, MCP9800_REG_TEMP, dst, 2);
	if (res)
		return res;
	return 2;
}


