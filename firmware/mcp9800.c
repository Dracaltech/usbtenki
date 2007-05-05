
#include <string.h>
#include "i2c.h"
#include "mcp9800.h"

int mcp9800_writeRegister(char i2c_addr, char reg_addr, unsigned char *dat, char len)
{
	unsigned char tmp[len+1];
	tmp[0] = reg_addr & 0x03;
	memcpy(&tmp[1], dat, len);
	return i2c_transaction(i2c_addr, len + 1, tmp, 0, NULL);
}

int mcp9800_readRegister(char i2c_addr, char reg_addr, unsigned char *dst, char len)
{
	unsigned char tmp;
	tmp = reg_addr & 0x03;
	return i2c_transaction(i2c_addr, 1, &tmp, len, dst);
}

int mcp9800_configure(char i2c_addr, unsigned char cfg)
{
	return mcp9800_writeRegister(i2c_addr, MCP9800_REG_CFG, &cfg, 1);
}

