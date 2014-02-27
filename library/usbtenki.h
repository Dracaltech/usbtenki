/* usbtenkiget: A command-line tool for reading USBTenki sensors.
 * Copyright (C) 2007-2014  Raphael Assenat <raph@raphnet.net>
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

//#include <usb.h>

#include "usbtenki_units.h"

#ifdef __cplusplus
extern "C" {
#endif



struct USBTenki_info {
	char str_prodname[256];
	char str_serial[256];
	int major, minor;
};

struct USBTenki_list_ctx;

struct USBTenki_channel {
	int channel_id;
	int chip_id;
	char data_valid;
	char saturated;
	unsigned char raw_data[8];
	int raw_length;
	float raw_value; // Not always used.
	float converted_data;
	int converted_unit;
};

#define USBTENKI_FLAG_NO_HUMIDEX_RANGE				1
#define USBTENKI_FLAG_NO_HEAT_INDEX_RANGE			2
#define USBTENKI_FLAG_USE_OLD_SHT75_COMPENSATION	4
#define USBTENKI_FLAG_VERBOSE						8

typedef void* USBTenki_dev_handle; // Cast from usb_dev_handle
typedef void* USBTenki_device; // Cast from usb_device

#define USBTENKI_DEV_TO_USB_DEVICE(d) ((struct usb_device*)(d))

int usbtenki_init(void);
void unsbtenki_shutdown(void);

struct USBTenki_list_ctx *usbtenki_allocListCtx(void);
void usbtenki_freeListCtx(struct USBTenki_list_ctx *ctx);

USBTenki_device usbtenki_listDevices(struct USBTenki_info *info,
										struct USBTenki_list_ctx *ctx);

USBTenki_dev_handle usbtenki_openDevice(USBTenki_device tdev);
USBTenki_dev_handle usbtenki_openBySerial(const char *serial, struct USBTenki_info *info);

void usbtenki_closeDevice(USBTenki_dev_handle hdl);


int usbtenki_command(USBTenki_dev_handle hdl, unsigned char cmd,
										int id, unsigned char *dst);
int usbtenki_getRaw(USBTenki_dev_handle hdl, int id, unsigned char *dst);
int usbtenki_getCalibration(USBTenki_dev_handle hdl, int id, unsigned char *dst);
int usbtenki_getChipID(USBTenki_dev_handle hdl, int id);
int usbtenki_getNumChannels(USBTenki_dev_handle hdl);

int usbtenki_convertRaw(struct USBTenki_channel *chn, unsigned long flags, unsigned char *caldata, int caldata_len);

int usbtenki_listChannels(USBTenki_dev_handle hdl, struct USBTenki_channel *dstArray, int arr_size);

void usbtenki_set_seaLevelStandardPressure(double slp_P); // in Pascals (default: 101325)
/**
 * \brief Add virtual channels based on previously listed ones (see usbtenki_listChannels)
 * \param chnArray array to hold the channels
 * \param num_channels The number of channels available in the arrary
 * \param arr_size The maximum number of channels that can be held by the array.
 */
int usbtenki_addVirtualChannels(struct USBTenki_channel *chnArray, int *num_channels, int arr_size);
int usbtenki_processVirtualChannels(USBTenki_dev_handle hdl, struct USBTenki_channel *channels, int num_channels, unsigned long flags);

int usbtenki_readChannelList(USBTenki_dev_handle hdl, int *channel_ids, int num, struct USBTenki_channel *dst, int dst_total, int num_attempts, unsigned long flags);
int usbtenki_readChannel(USBTenki_dev_handle hdl, struct USBTenki_channel *chn, unsigned long flags);

float usbtenki_convertTemperature(float temperature, int src_fmt, int dst_fmt);
float usbtenki_convertPressure(float pressure, int src_fmt, int dst_fmt);
float usbtenki_convertFrequency(float freq, int src_fmt, int dst_fmt);
float usbtenki_convertVoltage(float freq, int src_fmt, int dst_fmt);
float usbtenki_convertCurrent(float freq, int src_fmt, int dst_fmt);
float usbtenki_convertPower(float freq, int src_fmt, int dst_fmt);
const char *chipToString(int id);
const char *chipToShortString(int id);
const char *unitToString(int unit, int no_fancy_chars);

void usbtenki_convertUnits(struct USBTenki_channel *chn, int unit_temp, int unit_pressure, int unit_frequency, int voltage_unit, int current_unit, int power_unit);

#ifdef __cplusplus
}
#endif

#endif // _usbtenki_h__
