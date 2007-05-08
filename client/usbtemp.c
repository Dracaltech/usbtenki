/* usbtempctl: A command-line tool for reading raphnet.net's USB sensors.
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

#include "usbtemp.h"
#include "usbtemp_cmds.h"

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

void usbtemp_initListCtx(struct USBtemp_list_ctx *ctx)
{
	memset(ctx, 0, sizeof(struct USBtemp_list_ctx));
}

/**
 * \brief List instances of our usbtemp device on the USB busses.
 * \param dst Destination buffer for device serial number/id. 
 * \param dstbuf_size Destination buffer size.
 */
struct usb_device *usbtemp_listDevices(struct USBtemp_info *info, struct USBtemp_list_ctx *ctx)
{
	struct usb_bus *start_bus;
	struct usb_device *start_dev;

	memset(info, 0, sizeof(struct USBtemp_info));

	
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

					if (strcmp(info->str_prodname, ID_STRING)) {
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



static int usbtemp_command(usb_dev_handle *hdl, unsigned char cmd, 
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

int usbtemp_getRaw(usb_dev_handle *hdl, int id, unsigned char *dst)
{
	return usbtemp_command(hdl, USBTEMP_GET_RAW, id, dst);
}

int usbtemp_getNumChannels(usb_dev_handle *hdl)
{
	unsigned char dst[8];
	int res;

	res = usbtemp_command(hdl, USBTEMP_GET_NUM_CHANNELS, 0, dst);
	if (res<0)
		return res;
	if (res<1) /* Illegal for this command */
		return res;
	return dst[0];	
}

int usbtemp_getChipID(usb_dev_handle *hdl, int id)
{
	unsigned char dst[8];
	int res;

	res = usbtemp_command(hdl, USBTEMP_GET_CHIP_ID, id, dst);
	if (res<0)
		return res;
	if (res!=1) /* Illegal for this command */
		return res;
	return dst[0];	
}

static void printTempFmt(float temperature, int src_fmt, int print_fmt)
{
	float converted = temperature;

	switch (src_fmt)
	{
		case TEMPFMT_CELCIUS:
			switch(print_fmt)
			{
				case TEMPFMT_FAHRENHEIT:
					converted = (temperature * 1.8) + 32.0;
					break;
				case TEMPFMT_KELVIN:
					converted = temperature + 273.15;
					break;
			}
			break;

		case TEMPFMT_FAHRENHEIT:
			switch(print_fmt)
			{
				case TEMPFMT_CELCIUS:
					converted = (temperature - 32.0) / 1.8;
					break;
				case TEMPFMT_KELVIN:
					converted = (temperature + 459.67) / 1.8;
					break;
			}
			break;

		case TEMPFMT_KELVIN:
			switch(print_fmt)
			{
				case TEMPFMT_CELCIUS:
					converted = temperature - 273.15;

				case TEMPFMT_FAHRENHEIT:
					converted = (temperature * 1.8) - 459.67;
					break;
			}
			break;
	}
		
	printf("%.2f\n", converted);
}

int usbtemp_printTemperature(usb_dev_handle *hdl, int id, int fmt, int n_avg)
{
	unsigned char raw_data[8];
	int n_raw, i, chip, sample;
	float temperature;
	float accm;
	int chip_fmt = TEMPFMT_FAHRENHEIT;
	int is_humidity=0;

	chip = usbtemp_getChipID(hdl, id);
	if (chip<0) 
		return chip;
	
	if (g_verbose)
		printf("Chip id: 0x%02x\n", chip);

	accm = 0;
	for (sample=0; sample<n_avg; sample++)
	{
		n_raw = usbtemp_getRaw(hdl, id, raw_data);
		if (n_raw<0)
			return n_raw;

		switch (chip)
		{
			case USBTEMP_CHIP_MCP9800:
				{
					signed short t;

					if (n_raw!=2)
						goto wrongData;
					
					/* The sensor will be initailized in 12 bits mode  */
					t = (raw_data[0] << 4) | (raw_data[1]>>4);
					temperature = ((float)t) * pow(2.0,-4.0);
					chip_fmt = TEMPFMT_CELCIUS;
				}
				break;

			case USBTEMP_CHIP_LM75:
				{
					signed short t;

					if (n_raw!=2)
						goto wrongData;
					
					/* The sensor only supports 9 bits */
					t = (raw_data[0] << 1) | (raw_data[1]>>7);
					temperature = ((float)t) * pow(2.0,-1.0);
					chip_fmt = TEMPFMT_CELCIUS;

				}
				break;

			case USBTEMP_CHIP_SHT_TEMP:
				{
					unsigned  short t;

					if (n_raw!=2)
						goto wrongData;

					t = (raw_data[0]<<8) | raw_data[1];
//					temperature = ((float)t) * pow(2.0,-6.0);
				
					temperature = -40.0 + 0.01  * (float)t;

					chip_fmt = TEMPFMT_CELCIUS;
				}
				break;

			case USBTEMP_CHIP_SHT_RH:
				{
					float c1 = -4.0;
					float c2 = 0.0405;
					float c3 = -2.8 * powf(10.0, -6.0);
					float sorh;

					if (n_raw!=2)
						goto wrongData;

					sorh = (float)( (unsigned short)((raw_data[0]<<8) | raw_data[1]) );
				
					temperature = c1 + c2*sorh + c3 * powf(sorh, 2.0);

					chip_fmt = TEMPFMT_CELCIUS;
					is_humidity = 1;
				}


				break;

			default:
				printf("hex: ");
				for (i=0; i<n_raw; i++) {
					printf("%02x ", raw_data[i]);
				}
				printf("\n");
				return 0;
				break;

		}
		
		accm += temperature;
	}
	
	accm /= n_avg;

	if (is_humidity) {
		printf("%.2f\n", accm);		
	}
	else {
		printTempFmt(accm, chip_fmt, fmt);
	}

	return 0;

wrongData:
	fprintf(stderr, "Wrong data received\n");
	return -1;
}

const char *chipToString(int id)
{
	switch(id)
	{
		case USBTEMP_CHIP_MCP9800:
			return "MCP980x I2C Temperature sensor";
		case USBTEMP_CHIP_LM75:
			return "LM75 I2C Temperature sensor";
		case USBTEMP_CHIP_LM92:
			return "LM92 I2C Temperature sensor";
		case USBTEMP_CHIP_SHT_TEMP:
			return "Sensirion SHT1x/7x Temperature";
		case USBTEMP_CHIP_SHT_RH:
			return "Sensirion SHT1x/7x Relative Humidity";

	}
	return "unknown";
}

