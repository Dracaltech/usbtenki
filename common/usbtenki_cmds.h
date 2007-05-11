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

#define USBTENKI_CHIP_MCP9800	0x00
#define USBTENKI_CHIP_LM75		0x01
#define USBTENKI_CHIP_LM92		0x02
#define USBTENKI_CHIP_SHT_TEMP	0x03
#define USBTENKI_CHIP_SHT_RH	0x04

/* High channel numbers used in client. Not
 * real channels.. */
#define USBTENKI_VIRTUAL_START		0x100
#define USBTENKI_VIRTUAL_DEW_POINT	0x100
#define USBTENKI_VIRTUAL_HUMIDEX	0x101

#endif // _usbtenki_h__

