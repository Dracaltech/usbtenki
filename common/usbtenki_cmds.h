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


#define USBTENKI_CHIP_MCP9800	0x00
#define USBTENKI_CHIP_LM75		0x01
#define USBTENKI_CHIP_LM92		0x02
#define USBTENKI_CHIP_SHT_TEMP	0x03
#define USBTENKI_CHIP_SHT_RH	0x04

#define USBTENKI_MCU_ADC0		0x80
#define USBTENKI_MCU_ADC1		0x81
#define USBTENKI_MCU_ADC2		0x82
#define USBTENKI_MCU_ADC3		0x83
#define USBTENKI_MCU_ADC4		0x84
#define USBTENKI_MCU_ADC5		0x85

#define USBTENKI_CHIP_MPX4115	0x90

#define USBTENKI_CHIP_NONE		0xFF

/* High channel numbers used in client. Not
 * real channels.. */
#define USBTENKI_VIRTUAL_START		0x100
#define USBTENKI_VIRTUAL_DEW_POINT	0x100
#define USBTENKI_VIRTUAL_HUMIDEX	0x101
#define USBTENKI_VIRTUAL_HEAT_INDEX	0x102

#endif // _usbtenki_h__
