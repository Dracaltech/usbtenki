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
#include <stdio.h>
#include "interface.h"
#include "usbtenki_cmds.h"
#include "i2c.h"
#include "ina226.h"

// A1 and A0 are connected to GND. Hence slave address is 100 0000
static unsigned char chip_addr = 0x40;
static unsigned short g_calibration; // The INA226 calibration register

int sensors_init(void)
{
	unsigned short config;

	i2c_init(I2C_FLAG_INTERNAL_PULLUP, 255);

	usbtenki_delay_ms(10);

	// TODO: Those are the default values.
	config = 0; // 1 average
	config |= INA226_CFG_VBUS_CT2; // 1.1ms conversion time (bus voltage)
	config |= INA226_CFG_VSH_CT2; // 1.1ms conversion time (shunt voltage)
	config |= INA226_CFG_MODE1|INA226_CFG_MODE2|INA226_CFG_MODE3; // Measure Shunt and Bus, continuous

	if (ina226_writeRegister(chip_addr, INA226_REG_CONFIGURATION, config)) {
		return -1;
	}

	// Maximum supported current 20A
	// Shunt resistor: 0.01 Ohm
	// Shunt voltage at 20A: 20A * 0.01ohm = 0.2V
	//
	// Maximum voltage supported 30V (limited by on-board TVS)
	// Maximum power: 20A * 30V = 600W
	// Max dissipation: 2W
	// ----------
	//
	// Current_LSB = (max current) / 2^15 = 0.00061035156	[equation 2]
	// CAL = 0.00512 / (Current_LSB * R_shunt) = 838.8608	[equation 1]
	//
	//
	g_calibration = 839;
	if (ina226_writeRegister(chip_addr, INA226_REG_CALIBRATION, g_calibration)) {
		return -1;
	}

	return -1;
}

int sensors_getNumChannels(void)
{
	return 4;
}

int sensors_getChipID(unsigned char id)
{
	switch (id)
	{
		case 0: return USBTENKI_CHIP_INA226_BUS_VOLTAGE;
		case 1: return USBTENKI_CHIP_INA226_SHUNT_VOLTAGE;
		case 2: return USBTENKI_CHIP_INA226_POWER;
		case 3: return USBTENKI_CHIP_INA226_CURRENT;
	}
	return -1;
}

int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	unsigned short val;
	int res;

	switch (id)
	{
		case 0: res = ina226_readRegister(chip_addr, INA226_REG_BUS_VOLTAGE, &val); break;
		case 1: res = ina226_readRegister(chip_addr, INA226_REG_SHUNT_VOLTAGE, &val); break;
		case 2: res = ina226_readRegister(chip_addr, INA226_REG_POWER, &val); break;
		case 3: res = ina226_readRegister(chip_addr, INA226_REG_CURRENT, &val); break;
		default:
			return -1;
	}

	dst[0] = val >> 8;;
	dst[1] = val;
	dst[2] = g_calibration >> 8;
	dst[3] = g_calibration;
			
	return 2;
}


