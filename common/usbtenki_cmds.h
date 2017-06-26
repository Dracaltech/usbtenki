#ifndef _usbtenki_cmds_h__
#define _usbtenki_cmds_h__

#define USBTENKI_ERROR			0x00

/* Get sensor chip id/type.
 *   request: USBTENKI_GET_CHIP_ID
 *   value: Channel ID
 * Returned data:
 * [0x01] [id byte] [lrc]
 */
#define USBTENKI_GET_CHIP_ID			0x01

/* Get number of channels.
 *   request: USBTENKI_GET_NUM_CHANNELS
 *   value: Channel ID
 * Returned data:
 * [0x02] [num channel byte] [lrc]
 */
#define USBTENKI_GET_NUM_CHANNELS	0x02


/* Get raw data from sensor.
 * 	request: USBTENKI_GET_RAW
 * 	value: Channel ID
 * 
 * Returned data on error:
 * [-1]
 *
 * Returned data:
 * [0x10] [n bytes] [lrc]
 */
#define USBTENKI_GET_RAW			0x10

/* Get calibration data for channel.
 * 	request: USBTENKI_GET_CALIBRATION
 * 	value: Channel ID
 * 
 * Returned data on error:
 * [-1]
 *
 * Returned data:
 * [0x10] [n bytes] [lrc]
 */
#define USBTENKI_GET_CALIBRATION	0x11

/* Set the serial number characters one by one.
 *   request: USBTENKI_SET_SERIAL
 *   value: Low byte is character index. 
 *   		0xff means done, write to eeprom and become live.
 *   		High byte is the character itself.
 *
 * Returned data:
 * [0xf0] [lrc]
 */
#define USBTENKI_SET_SERIAL			0xf0

/* Set the chip type that corresponds to an
 * ADC channel (on the MCU).
 *
 *   request: USBTENKI_SET_ADC_CHIP
 *   value: Low byte is adc channel (0-5)
 *          High byte is the chipid. (or Hidden)
 *
 * Returned data:
 * [0xf1] [lrc]
 */
#define USBTENKI_SET_ADC_CHIP		0xf1

/* Select the ADC reference to use.
 *
 *   request: USBTENKI_SET_ADC_CHIP
 *   value: Low byte (0=internal 5v, 1=adc ref)
 *
 * Returned data:
 * [0xf2] [lrc]
 */
#define USBTENKI_SET_ADC_REF		0xf2


/* Set the RTD current source calibration
 *
 *   request: USBTENKI_SET_RTD_CORR
 *   value: percent * 100 (one signed byte)
 *
 * Returned data:
 * [0xf2] [lrc]
 */
#define USBTENKI_SET_RTD_CORR		0xf3

#define USBTENKI_SET_EM1_CALIBRATION	0xf4
#define USBTENKI_SET_EM1_MAX_CURRENT	0xf5
#define USBTENKI_SET_MISC1_CALIBRATION	0xf4

// Sets Zero or point of origin depending on sensor.
#define USBTENKI_ZERO					0xf6

/* Set poll rate for SHT31.
 *   request: USBTENKI_GET_NUM_CHANNELS
 *   value: Rate, where
 *
 * 0: 0.5 sample per second
 * 1: 1 sample per second
 * 2: 2 samples per second
 * 3: 4 samples per second
 * 4: 10 samples per second
 *
 * Returned data:
 * [0xf6] [lrc]
 */
#define USBTENKI_SET_SHT31_RATE		0xf7


/* Command the device to enter bootloader mode.
 *
 *   request: USBTENKI_BOOTLOADER
 *   value: 0xB007
 *
 * No return (MCU enters bootloader and does not answer)
 */
#define USBTENKI_BOOTLOADER				0xFF

#define USBTENKI_CHIP_MCP9800	0x00
#define USBTENKI_CHIP_LM75		0x01
#define USBTENKI_CHIP_LM92		0x02
#define USBTENKI_CHIP_SHT_TEMP	0x03
#define USBTENKI_CHIP_SHT_RH	0x04
#define USBTENKI_CHIP_TSL2561_IR_VISIBLE	0x05
#define USBTENKI_CHIP_TSL2561_IR	0x06

#define USBTENKI_CHIP_BS02_TEMP		0x07
#define USBTENKI_CHIP_BS02_RH		0x08

#define USBTENKI_CHIP_TSL2561_IR_VISIBLE_16X	0x09
#define USBTENKI_CHIP_TSL2561_IR_16X			0x0A

#define USBTENKI_CHIP_TSL2568_IR_VISIBLE		0x0B
#define USBTENKI_CHIP_TSL2568_IR_VISIBLE_16X	0x0C
#define USBTENKI_CHIP_TSL2568_IR				0x0D
#define USBTENKI_CHIP_TSL2568_IR_16X			0x0E

#define USBTENKI_CHIP_SE95			0x0F
#define USBTENKI_CHIP_D6F_V03A1		0x10
#define USBTENKI_CHIP_ADT7410		0x11

#define USBTENKI_CHIP_PT100_RTD		0x12
#define USBTENKI_CHIP_MLX90614_TA	0x13
#define USBTENKI_CHIP_MLX90614_TOBJ	0x14
#define USBTENKI_CHIP_MS5611_P		0x15
#define USBTENKI_CHIP_MS5611_T		0x16

#define USBTENKI_CHIP_CC2_RH		0x17
#define USBTENKI_CHIP_CC2_T			0x18
#define USBTENKI_CHIP_SHT31_RH		0x19
#define USBTENKI_CHIP_SHT35_RH		0x1A
#define USBTENKI_CHIP_SHT35_T		0x1B

#define USBTENKI_CHIP_SHT31_T		0x20

#define USBTENKI_CHIP_DRACAL_EM1_BUS_VOLTAGE    0x30
#define USBTENKI_CHIP_DRACAL_EM1_SHUNT_VOLTAGE  0x31
#define USBTENKI_CHIP_DRACAL_EM1_POWER          0x32
#define USBTENKI_CHIP_DRACAL_EM1_CURRENT        0x33

#define USBTENKI_CHIP_CO2_PPM		0x40

#define USBTENKI_MCU_ADC0		0x80
#define USBTENKI_MCU_ADC1		0x81
#define USBTENKI_MCU_ADC2		0x82
#define USBTENKI_MCU_ADC3		0x83
#define USBTENKI_MCU_ADC4		0x84
#define USBTENKI_MCU_ADC5		0x85


#define USBTENKI_CHIP_MPX4115	0x90

/* Ratiometric volts measurement based on 5 volts avcc */
#define USBTENKI_CHIP_VOLTS		0x91
#define USBTENKI_CHIP_VOLTS_REVERSE	0x92

#define USBTENKI_CHIP_MP3H6115A	0x93
#define USBTENKI_CHIP_MPXV7002	0x94

/* Honeywell MLH150 xxx x x x xx A pressure sensor. */
#define USBTENKI_CHIP_MLH_A		0x95

#define USBTENKI_CHIP_TACHOMETER	0xA0

#define USBTENKI_CHIP_NONE		0xFF

/* High channel numbers used in client. Not
 * real channels.. */
#define USBTENKI_VIRTUAL_START		0x100
#define USBTENKI_VIRTUAL_DEW_POINT	0x100
#define USBTENKI_VIRTUAL_HUMIDEX	0x101
#define USBTENKI_VIRTUAL_HEAT_INDEX	0x102
#define USBTENKI_VIRTUAL_TSL2561_LUX 0x103
#define USBTENKI_VIRTUAL_TSL2568_LUX 0x104
#define USBTENKI_VIRTUAL_SHT75_COMPENSATED_RH	0x105
#define USBTENKI_VIRTUAL_ALTITUDE	0x106

#endif // _usbtenki_h__

