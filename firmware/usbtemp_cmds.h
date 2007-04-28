#ifndef _usbtemp_cmds_h__
#define _usbtemp_cmds_h__

#define USBTEMP_ERROR			0x00

/* Get sensor chip id/type.
 *   request: USBTEMP_GET_CHIP_ID
 *   value: Channel ID
 * Returned data:
 * [0x01] [id byte] [lrc]
 */
#define USBTEMP_GET_CHIP_ID			0x01

/* Get number of channels.
 *   request: USBTEMP_GET_NUM_CHANNELS
 *   value: Channel ID
 * Returned data:
 * [0x02] [num channel byte] [lrc]
 */
#define USBTEMP_GET_NUM_CHANNELS	0x02

/* Get raw data from sensor.
 * 	request: USBTEMP_GET_RAW
 * 	value: Channel ID
 * 
 * Returned data on error:
 * [-1]
 *
 * Returned data:
 * [0x10] [n bytes] [lrc]
 */
#define USBTEMP_GET_RAW			0x10

#define USBTEMP_CHIP_MCP9800	0x00
//#define USBTEMP_CHIP_LM75		0x01
//#define USBTEMP_CHIP_LM92		0x02

#endif // _usbtemp_h__

