#ifndef _i2c_h__
#define _i2c_h__

#define I2C_FLAG_INTERNAL_PULLUP	0x1
#define I2C_FLAG_EXTERNAL_PULLUP	0x0

void i2c_init(int flags, unsigned char twbr);
int i2c_transaction(unsigned char addr, int wr_len, unsigned char *wr_data, 
								int rd_len, unsigned char *rd_data);

#endif // _i2c_h__


