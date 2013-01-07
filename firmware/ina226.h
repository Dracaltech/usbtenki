/*   USBTenki - Interfacing sensors to USB 
 *   Copyright (C) 2007-2013  Raphaël Assénat <raph@raphnet.net>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _ina226_h__
#define _ina226_h__

#define INA226_REG_CONFIGURATION	0x00
#define INA226_REG_SHUNT_VOLTAGE	0x01
#define INA226_REG_BUS_VOLTAGE		0x02
#define INA226_REG_POWER			0x03
#define INA226_REG_CURRENT			0x04
#define INA226_REG_CALIBRATION		0x05
#define INA226_REG_MASK				0x06
#define INA226_REG_ALERT			0x07
#define INA226_REG_DIE_ID			0xFF

#define INA226_CFG_RST				0x8000
#define INA226_CFG_AVG2				0x0800
#define INA226_CFG_AVG1				0x0400
#define INA226_CFG_AVG0				0x0200
#define INA226_CFG_VBUS_CT2			0x0100
#define INA226_CFG_VBUS_CT1			0x0080
#define INA226_CFG_VBUS_CT0			0x0040
#define INA226_CFG_VSH_CT2			0x0020
#define INA226_CFG_VSH_CT1			0x0010
#define INA226_CFG_VSH_CT0			0x0008
#define INA226_CFG_MODE3			0x0004
#define INA226_CFG_MODE2			0x0002
#define INA226_CFG_MODE1			0x0001

int ina226_readRegister(unsigned char i2c_addr, unsigned char reg, unsigned short *dst_value);
int ina226_writeRegister(unsigned char i2c_addr, unsigned char reg, unsigned short value);

#endif // _ina226_h__
