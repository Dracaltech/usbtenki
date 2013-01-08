#include <stdio.h>
#include "i2c.h"
#include "ina226.h"

int ina226_writeRegister(unsigned char i2c_addr, unsigned char reg, unsigned short value)
{
	unsigned char tmp[3];

	tmp[0] = reg;
	tmp[1] = value >> 8;
	tmp[2] = value;

	return i2c_transaction(i2c_addr, 3, tmp, 0, NULL);
}

int ina226_readRegister(unsigned char i2c_addr, unsigned char reg, unsigned short *dst_value)
{
	unsigned char tmp[2];
	int res;

	if (!dst_value)
		return -1;

	tmp[0] = reg;
	
	res = i2c_transaction(i2c_addr, 1, &tmp, 2, tmp);
	if (res == 0) {
		*dst_value = (tmp[0]<<8) | tmp[1];
	}

	return res;
}
