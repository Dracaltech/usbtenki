/* usbtenkiget: A command-line tool for reading USBTenki sensors.
 * Copyright (C) 2007  Raphael Assenat <raph@raphnet.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef _usbtenki_h__
#define _usbtenki_h__

#include <usb.h>

#define OUR_VENDOR_ID 	0x1781
#define OUR_PRODUCT_ID 	0x0a98
#define ID_STRING		"USBTenki"
#define OLD_ID_STRING	"USB_Temp"

#define TENKI_UNIT_RAW			0x00

#define TENKI_UNIT_CELCIUS		0x01
#define TENKI_UNIT_FAHRENHEIT	0x02
#define TENKI_UNIT_KELVIN		0x03
#define TENKI_UNIT_RH			0x04

#define TENKI_UNIT_KPA			0x10
#define TENKI_UNIT_HPA			0x11
#define TENKI_UNIT_BAR			0x12
#define TENKI_UNIT_AT			0x13
#define TENKI_UNIT_ATM			0x14
#define TENKI_UNIT_TORR			0x15
#define TENKI_UNIT_PSI			0x16


struct USBTenki_info {
	char str_prodname[256];
	char str_serial[256];
	int major, minor;
};

struct USBTenki_list_ctx {
	struct usb_bus *bus;
	struct usb_device *dev;
};

struct USBTenki_channel {
	int channel_id;
	int chip_id;
	char data_valid;
	unsigned char raw_data[8];
	int raw_length;
	float converted_data;
	int converted_unit;
};

void usbtenki_initListCtx(struct USBTenki_list_ctx *ctx);
struct usb_device *usbtenki_listDevices(struct USBTenki_info *info, 
										struct USBTenki_list_ctx *ctx);

int usbtenki_command(usb_dev_handle *hdl, unsigned char cmd, 
										int id, unsigned char *dst);
int usbtenki_getRaw(usb_dev_handle *hdl, int id, unsigned char *dst);
int usbtenki_getChipID(usb_dev_handle *hdl, int id);
int usbtenki_getNumChannels(usb_dev_handle *hdl);

int usbtenki_convertRaw(struct USBTenki_channel *chn);

int usbtenki_listChannels(usb_dev_handle *hdl, struct USBTenki_channel *dstArray, int arr_size);
int usbtenki_readChannelList(usb_dev_handle *hdl, int *channel_ids, int num, struct USBTenki_channel *dst, int dst_total, int num_attempts);
int usbtenki_readChannel(usb_dev_handle *hdl, struct USBTenki_channel *chn);

float usbtenki_convertTemperature(float temperature, int src_fmt, int dst_fmt);
float usbtenki_convertPressure(float pressure, int src_fmt, int dst_fmt);
const char *chipToString(int id);
const char *chipToShortString(int id);
const char *unitToString(int unit, int no_fancy_chars);

#endif // _rgbleds_h__

