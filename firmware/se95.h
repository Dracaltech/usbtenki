#ifndef _se95_h__
#define _se95_h__

// MSB LSB
// 1  0  0  1  A2  A1  A0
#define SE95_ADDR_BASE	0x48

#define SE95_REG_TEMP	0x00
#define SE95_REG_CONF	0x01
#define SE95_REG_THYST	0x02
#define SE95_REG_TOS	0x03
#define SE95_REG_ID		0x05

#define SE95_CFG_SHDN			0x00
#define SE95_CFG_OS_COMP_INIT	0x01
#define SE95_CFG_OS_POL			0x02
#define SE95_CFG_OS_F_QUE	 	0x03 // 2bit
#define SE95_CFG_RATEVAL		0x05 // 2 bits

#endif // _se95_h__

