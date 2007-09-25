#include <string.h>
#include "tsl2561.h"
#include "i2c.h"

int tsl2561_writeRegister(char i2c_addr, char reg_addr, unsigned char *dat, char len)
{
	unsigned char tmp[len+1];
	tmp[0] = reg_addr;
	memcpy(&tmp[1], dat, len);
	return i2c_transaction(i2c_addr, len + 1, tmp, 0, NULL);
}

int tsl2561_readRegister(char i2c_addr, char reg_addr, unsigned char *dst, char len)
{
	unsigned char tmp = reg_addr;
	return i2c_transaction(i2c_addr, 1, &tmp, len, dst);
}



