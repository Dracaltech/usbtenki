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
#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <math.h>

#include "usbtenki.h"
#include "usbtenki_cmds.h"

extern int g_verbose; // from main.c

static unsigned char xor_buf(unsigned char *buf, int len)
{
	unsigned char x=0;
	while (len--) {
		x ^= *buf;
		buf++;
	}
	return x;
}

void usbtenki_initListCtx(struct USBTenki_list_ctx *ctx)
{
	memset(ctx, 0, sizeof(struct USBTenki_list_ctx));
}

/**
 * \brief List instances of our usbtenki device on the USB busses.
 * \param dst Destination buffer for device serial number/id. 
 * \param dstbuf_size Destination buffer size.
 */
struct usb_device *usbtenki_listDevices(struct USBTenki_info *info, struct USBTenki_list_ctx *ctx)
{
	struct usb_bus *start_bus;
	struct usb_device *start_dev;

	memset(info, 0, sizeof(struct USBTenki_info));

	
	if (ctx->dev && ctx->bus)
		goto jumpin;
		
	start_bus = usb_get_busses();

	for (ctx->bus = start_bus; ctx->bus; ctx->bus = ctx->bus->next) {
		
		start_dev = ctx->bus->devices;
		for (ctx->dev = start_dev; ctx->dev; ctx->dev = ctx->dev->next) {
			if (ctx->dev->descriptor.idVendor == OUR_VENDOR_ID) {
				if (ctx->dev->descriptor.idProduct == OUR_PRODUCT_ID) {
					usb_dev_handle *hdl;
					hdl = usb_open(ctx->dev);
					if (!hdl)
						continue; 
					
					usb_get_string_simple(hdl, ctx->dev->descriptor.iProduct,
										info->str_prodname, 256);

					if (strcmp(info->str_prodname, ID_STRING) &&
						strcmp(info->str_prodname, OLD_ID_STRING)) {
						if (g_verbose)
							printf("Ignored: %s\n", info->str_prodname);

						usb_close(hdl);
						continue;
					}

					usb_get_string_simple(hdl, ctx->dev->descriptor.iSerialNumber,
										info->str_serial, 256);
					info->minor = ctx->dev->descriptor.bcdDevice & 0xff;
					info->major = (ctx->dev->descriptor.bcdDevice & 0xff00) >> 8;

					usb_close(hdl);
					return ctx->dev;
				}
			}
			
jumpin:
			// prevent 'error: label at end of compound statement' 
			continue;
		}
	}

	return NULL;
}



int usbtenki_command(usb_dev_handle *hdl, unsigned char cmd, 
										int id, unsigned char *dst)
{
	unsigned char buffer[8];
	unsigned char xor;
	int n;
	int datlen;

	n =	usb_control_msg(hdl, 
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, /* requesttype */
		cmd, 	/* request*/
		id, 				/* value */
		0, 					/* index */
		(char*)buffer, sizeof(buffer), 5000);

	/* Validate size first */
	if (n>8) {
		fprintf(stderr, "Too much data received! (%d)\n", n);
		return -3;
	} else if (n<2) {
		fprintf(stderr, "Not enough data received! (%d)\n", n);
		return -4;	
	}
	
	/* dont count command and xor */
	datlen = n - 2;

	/* Check if reply is for this command */
	if (buffer[0] != cmd) {
		fprintf(stderr, "Wrong reply received (0x%02x)\n", buffer[0]);
		return -5;
	}

	/* Check xor */
	xor = xor_buf(buffer, n);
	if (xor) {
		fprintf(stderr, "Communication corruption occured!\n");
		return -2;
	}

	if (datlen) {
		memcpy(dst, buffer+1, datlen);
	}

	return datlen;
}

int usbtenki_getRaw(usb_dev_handle *hdl, int id, unsigned char *dst)
{
	return usbtenki_command(hdl, USBTENKI_GET_RAW, id, dst);
}

int usbtenki_getNumChannels(usb_dev_handle *hdl)
{
	unsigned char dst[8];
	int res;

	res = usbtenki_command(hdl, USBTENKI_GET_NUM_CHANNELS, 0, dst);
	if (res<0)
		return res;
	if (res<1) /* Illegal for this command */
		return res;
	return dst[0];	
}

int usbtenki_getChipID(usb_dev_handle *hdl, int id)
{
	unsigned char dst[8];
	int res;

	res = usbtenki_command(hdl, USBTENKI_GET_CHIP_ID, id, dst);
	if (res<0)
		return res;
	if (res!=1) /* Illegal for this command */
		return res;
	return dst[0];	
}

float usbtenki_convertTemperature(float temperature, int src_fmt, int dst_fmt)
{
	float converted = temperature;

	switch (src_fmt)
	{
		case TENKI_UNIT_CELCIUS:
			switch(dst_fmt)
			{
				case TENKI_UNIT_FAHRENHEIT:
					converted = (temperature * 1.8) + 32.0;
					break;
				case TENKI_UNIT_KELVIN:
					converted = temperature + 273.15;
					break;
			}
			break;

		case TENKI_UNIT_FAHRENHEIT:
			switch(dst_fmt)
			{
				case TENKI_UNIT_CELCIUS:
					converted = (temperature - 32.0) / 1.8;
					break;
				case TENKI_UNIT_KELVIN:
					converted = (temperature + 459.67) / 1.8;
					break;
			}
			break;

		case TENKI_UNIT_KELVIN:
			switch(dst_fmt)
			{
				case TENKI_UNIT_CELCIUS:
					converted = temperature - 273.15;

				case TENKI_UNIT_FAHRENHEIT:
					converted = (temperature * 1.8) - 459.67;
					break;
			}
			break;
	}

	return converted;
//	printf("%.2f\n", converted);
}

int usbtenki_convertRaw(struct USBTenki_channel *chn)
{
	float temperature;
	int chip_fmt = TENKI_UNIT_KELVIN;
	unsigned char *raw_data;

	raw_data = chn->raw_data;

	switch (chn->chip_id)
	{
		case USBTENKI_CHIP_MCP9800:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
				
				/* The sensor will be initailized in 12 bits mode  */
				t = (raw_data[0] << 4) | (raw_data[1]>>4);
				temperature = ((float)t) * pow(2.0,-4.0);
				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_LM75:
			{
				signed short t;

				if (chn->raw_length!=2)
					goto wrongData;
				
				/* The sensor only supports 9 bits */
				t = (raw_data[0] << 1) | (raw_data[1]>>7);
				temperature = ((float)t) * pow(2.0,-1.0);
				chip_fmt = TENKI_UNIT_CELCIUS;

			}
			break;

		case USBTENKI_CHIP_SHT_TEMP:
			{
				unsigned  short t;

				if (chn->raw_length!=2)
					goto wrongData;

				t = (raw_data[0]<<8) | raw_data[1];
//					temperature = ((float)t) * pow(2.0,-6.0);
			
				temperature = -40.0 + 0.01  * (float)t;

				chip_fmt = TENKI_UNIT_CELCIUS;
			}
			break;

		case USBTENKI_CHIP_SHT_RH:
			{
				float c1 = -4.0;
				float c2 = 0.0405;
				float c3 = -2.8 * powf(10.0, -6.0);
				float sorh;

				if (chn->raw_length!=2)
					goto wrongData;

				sorh = (float)( (unsigned short)((raw_data[0]<<8) | raw_data[1]) );
			
				temperature = c1 + c2*sorh + c3 * powf(sorh, 2.0);
				chip_fmt = TENKI_UNIT_RH;
			}
			break;

		case USBTENKI_CHIP_MPX4115:
			{
				float vout, vs, p;
				unsigned short adc_out;

				/* -- Sensor formulas:
				 *   "Vout = Vs * (.009 * P -0.095)"
				 *   where Vs is 5.1 Vdc. Output is ratiometric
				 *   to Vs between 4.85 and 5.35 volts.
				 *
				 * -- Atmel adc:
				 *   In 10 bit mode, 0x000 represents ground and 0x3ff represents
				 *   the selected reference voltage (VCC in our case) minus one
				 *   LSB.
				 * 
				 * The ADC reference voltage is the same as the sensor's Vs,
				 * So Vs does not really matter here.
				 */
				vs = 5.0;
				adc_out = raw_data[0] << 8 | raw_data[1];
				vout = (adc_out * vs) / 1024.0;
				p = ((vout/vs)+0.095)/.009;

				temperature = p;
				chip_fmt = TENKI_UNIT_KPA;
			}
			break;

		case USBTENKI_MCU_ADC0:
		case USBTENKI_MCU_ADC1:
		case USBTENKI_MCU_ADC2:
		case USBTENKI_MCU_ADC3:
		case USBTENKI_MCU_ADC4:
		case USBTENKI_MCU_ADC5:
			temperature = raw_data[0] << 8 | raw_data[1];
			chip_fmt = TENKI_UNIT_RAW;
			break;

		default:
			temperature = raw_data[0] << 8 | raw_data[1];
			chip_fmt = TENKI_UNIT_RAW;
			//printf("Unknown chip id 0x%02x\n", chn->chip_id);
			break;
	}

	chn->converted_data = temperature;
	chn->converted_unit = chip_fmt;

	return 0;

wrongData:
	fprintf(stderr, "Wrong data received\n");
	return -1;
}


const char *chipToString(int id)
{
	switch(id)
	{
		case USBTENKI_CHIP_MCP9800:
			return "MCP980x I2C Temperature sensor";
		case USBTENKI_CHIP_LM75:
			return "LM75 I2C Temperature sensor";
		case USBTENKI_CHIP_LM92:
			return "LM92 I2C Temperature sensor";
		case USBTENKI_CHIP_SHT_TEMP:
			return "Sensirion SHT1x/7x Temperature";
		case USBTENKI_CHIP_SHT_RH:
			return "Sensirion SHT1x/7x Relative Humidity";

		case USBTENKI_MCU_ADC0:
			return "Microcontroller ADC channel 0";
		case USBTENKI_MCU_ADC1:
			return "Microcontroller ADC channel 1";
		case USBTENKI_MCU_ADC2:
			return "Microcontroller ADC channel 2";
		case USBTENKI_MCU_ADC3:
			return "Microcontroller ADC channel 3";
		case USBTENKI_MCU_ADC4:
			return "Microcontroller ADC channel 4";
		case USBTENKI_MCU_ADC5:
			return "Microcontroller ADC channel 5";

		case USBTENKI_CHIP_MPX4115:
			return "MPX4115 Absolute air pressure sensor";

		/* Virtual channels and chipID have the same vales */
		case USBTENKI_VIRTUAL_DEW_POINT:
			return "Dew point";
		case USBTENKI_VIRTUAL_HUMIDEX:
			return "Humidex";
		case USBTENKI_VIRTUAL_HEAT_INDEX:
			return "Heat index";

	}
	return "unknown";
}

const char *chipToShortString(int id)
{
	switch(id)
	{
		case USBTENKI_CHIP_MCP9800:
		case USBTENKI_CHIP_LM75:
		case USBTENKI_CHIP_LM92:
		case USBTENKI_CHIP_SHT_TEMP:
			return "Temperature";
		
		case USBTENKI_CHIP_SHT_RH:
			return "Relative Humidity";
		
		case USBTENKI_MCU_ADC0:
		case USBTENKI_MCU_ADC1:
		case USBTENKI_MCU_ADC2:
		case USBTENKI_MCU_ADC3:
		case USBTENKI_MCU_ADC4:
		case USBTENKI_MCU_ADC5:
			return "Raw ADC output";

		case USBTENKI_CHIP_MPX4115:
			return "Pressure";

		/* Virtual channels and chipID have the same vales */
		case USBTENKI_VIRTUAL_DEW_POINT:
			return "Dew point";
		case USBTENKI_VIRTUAL_HUMIDEX:
			return "Humidex";
		case USBTENKI_VIRTUAL_HEAT_INDEX:
			return "Heat index";

	}
	return "unknown";
}

const char *unitToString(int unit)
{
	switch(unit)
	{
		case TENKI_UNIT_RH: return "%";
		case TENKI_UNIT_CELCIUS: return "°C";
		case TENKI_UNIT_KELVIN: return "°K";
		case TENKI_UNIT_FAHRENHEIT: return "°F";
		case TENKI_UNIT_RAW: return "(raw)";
		case TENKI_UNIT_KPA: return "kPa";
	}

	return "";
}

/**
 * \param hdl Handle
 * \param channel_ids Array of channel IDs
 * \param num The number of channel IDs
 * \param dst The destination array
 * \param dst_total The total number of channels in 'dst'
 *
 * dst must have been setup by usbtenki_listChannels() first!
 */
int usbtenki_readChannelList(usb_dev_handle *hdl, int *channel_ids, int num, struct USBTenki_channel *dst, int dst_total)
{
	int i, j, res;

	for (i=0; i<num; i++)
	{
		// skip virtual channels
		if (channel_ids[i]>=USBTENKI_VIRTUAL_START)
			continue;

		// Look for the destination
		for (j=0; j<dst_total; j++) {
			if (channel_ids[i] == dst[j].channel_id) 
				break;
		}
		if (j==dst_total) {
			fprintf(stderr, "Invalid channel ID (%d) requested\n", channel_ids[i]);
			continue;
		}

		if (dst[j].data_valid)
			continue; /* already done */

		dst[j].raw_length = usbtenki_getRaw(hdl, dst[j].channel_id, dst[j].raw_data);
		if (dst[j].raw_length<0)
			return dst[j].raw_length;		
		
		dst[j].data_valid = 1;
		res = usbtenki_convertRaw(&dst[j]);
		if (res==-1) {
			fprintf(stderr, "Failed to convert raw value from chip %d, channel: %d\n",
								dst[j].chip_id, dst[j].channel_id);
		}
	}

	return 0;
}

int usbtenki_listChannels(usb_dev_handle *hdl, struct USBTenki_channel *dstArray, int arr_size)
{
	int n_channels;
	int i;

	n_channels = usbtenki_getNumChannels(hdl);
	for (i=0; i<n_channels && i<arr_size; i++){
		dstArray->channel_id = i;
		dstArray->chip_id = usbtenki_getChipID(hdl, i);
		dstArray->data_valid = 0;

		dstArray++;
	}

	if (n_channels > arr_size)
		fprintf(stderr, "warning: Channel list truncated\n");

	return n_channels;
}


