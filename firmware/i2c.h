#ifndef _i2c_h__
#define _i2c_h__

void i2c_init(void);
int i2c_transaction(unsigned char addr, int wr_len, unsigned char *wr_data, 
								int rd_len, unsigned char *rd_data);

#endif // _i2c_h__


