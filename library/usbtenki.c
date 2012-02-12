/* usbtenki: A library for accessing USBTenki sensors.
 * Copyright (C) 2007-2011  Raphael Assenat <raph@raphnet.net>
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
//#include <usb.h>
#include <math.h>

#include "usbtenki.h"
#include "usbtenki_priv.h"
#include "usbtenki_cmds.h"
#include "usbtenki_units.h"

#ifdef WINDOWS_VERSION
#include <windows.h>
#define usleep(t) Sleep(t/1000)
#endif

int g_usbtenki_verbose=0;
int g_usbtenki_num_attempts = 3;

int usbtenki_init(void)
{
	usb_init();
	return 0;
}

void unsbtenki_shutdown(void)
{
}

static unsigned char xor_buf(unsigned char *buf, int len)
{
	unsigned char x=0;
	while (len--) {
		x ^= *buf;
		buf++;
	}
	return x;
}

static void usbtenki_initListCtx(struct USBTenki_list_ctx *ctx)
{
	memset(ctx, 0, sizeof(struct USBTenki_list_ctx));
}

struct USBTenki_list_ctx *usbtenki_allocListCtx(void)
{
	struct USBTenki_list_ctx *ctx;
	ctx = calloc(1, sizeof(struct USBTenki_list_ctx));
	return ctx;	
}

void usbtenki_freeListCtx(struct USBTenki_list_ctx *ctx)
{
	if (ctx)
		free(ctx);
}

/**
 * \brief List instances of our usbtenki device on the USB busses.
 * \param dst Destination buffer for device serial number/id. 
 * \param dstbuf_size Destination buffer size.
 */
USBTenki_device usbtenki_listDevices(struct USBTenki_info *info, struct USBTenki_list_ctx *ctx)
{
	struct usb_bus *start_bus;
	struct usb_device *start_dev;

	memset(info, 0, sizeof(struct USBTenki_info));

	
	if (ctx->dev && ctx->bus)
		goto jumpin;
		
	if (g_usbtenki_verbose)
			printf("Start listing\n");

	usb_find_busses();
	usb_find_devices();
	start_bus = usb_get_busses();

	if (start_bus==NULL) {
		if (g_usbtenki_verbose) {
			printf("No busses found!\n");
		}
		return NULL;
	}

	for (ctx->bus = start_bus; ctx->bus; ctx->bus = ctx->bus->next) {
		
		start_dev = ctx->bus->devices;
		for (ctx->dev = start_dev; ctx->dev; ctx->dev = ctx->dev->next) {
			if (ctx->dev->descriptor.idVendor == OUR_VENDOR_ID) {
				if (ctx->dev->descriptor.idProduct == OUR_PRODUCT_ID) {
					usb_dev_handle *hdl;
					hdl = usb_open(ctx->dev);
					if (!hdl) {
						if (g_usbtenki_verbose)
							printf("Failed to open device. Error '%s'\n", usb_strerror());
						continue; 
					}
					
					usb_get_string_simple(hdl, ctx->dev->descriptor.iProduct,
										info->str_prodname, 256);

					if (strcmp(info->str_prodname, ID_STRING) &&
						strcmp(info->str_prodname, OLD_ID_STRING)) {
						if (g_usbtenki_verbose)
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

USBTenki_dev_handle usbtenki_openDevice(USBTenki_device tdev)
{
	struct usb_dev_handle *hdl;
	int res;

	hdl = usb_open(tdev);
	if (!hdl)
		return NULL;

	res = usb_claim_interface(hdl, 0);
	if (res<0) {
		usb_close(hdl);
		return NULL;
	}

	return hdl;
}

void usbtenki_closeDevice(USBTenki_dev_handle hdl)
{
	usb_release_interface(hdl, 0);
	usb_close(hdl);
}

/**
 * \brief Search for an USBTenki device with a specific serial and open it.
 * \param serial Case-sensitive serial number
 * \param info Pointer to store device info. Pass a NULL if you don't need it.
 */
USBTenki_dev_handle usbtenki_openBySerial(const char *serial, struct USBTenki_info *info)
{
	struct USBTenki_list_ctx devlistctx;
	struct USBTenki_info inf;
	usb_dev_handle *hdl;
	struct usb_device *cur_dev, *dev=NULL;
	int res;

	usb_find_busses();
	usb_find_devices();
	
	usbtenki_initListCtx(&devlistctx);
	
	while ((cur_dev=usbtenki_listDevices(&inf, &devlistctx)))
	{
		if (strcmp(serial, inf.str_serial)==0) {
			if (info) {
				memcpy(info, &inf, sizeof (struct USBTenki_info));
			}
			dev = cur_dev;
			if (g_usbtenki_verbose) {
				printf("usbtenki.c: Found device '%s'\n", serial);
			}
			break;
		}
	}
	
	if (!dev)
		return NULL;

	hdl = usb_open(dev);
	if (!hdl) {
		fprintf(stderr, "usbtenki.c: USB_Error: %s\n", usb_strerror());
		return NULL;
	}


	if (g_usbtenki_verbose)
		printf("usbtenki.c: Setting configuration\n");
	
	res = usb_set_configuration(hdl, cur_dev->config->bConfigurationValue);
	if (res < 0) {
		printf("USB Error (usb_set_configuration: %s)\n", usb_strerror());
		usb_close(hdl);
		return NULL;
	}

	if (g_usbtenki_verbose)
		printf("usbtenki.c: Claiming interface\n");
	
	res = usb_claim_interface(hdl, 0);
	if (res < 0) {
		fprintf(stderr, "usbtenki.c: USB Error (usb_claim_interface: %s)\n", usb_strerror());
		usb_close(hdl);
		return NULL;
	}

	return (USBTenki_dev_handle) hdl;

}

int usbtenki_command(USBTenki_dev_handle hdl, unsigned char cmd, 
										int id, unsigned char *dst)
{
	unsigned char buffer[8];
	unsigned char xor;
	int n, i;
	int datlen;
	static int first = 1, trace = 0;

	if (first) {
		if (getenv("USBTENKI_TRACE")) {
			trace = 1;
		}
		first = 0;
	}
	

	n =	usb_control_msg(hdl, 
		USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, /* requesttype */
		cmd, 	/* request*/
		id, 				/* value */
		0, 					/* index */
		(char*)buffer, sizeof(buffer), 5000);

	if (trace) {
		printf("req: 0x%02x, val: 0x%02x, idx: 0x%02x <> %d: ",
			cmd, id, 0, n);
		if (n>0) {
			for (i=0; i<n; i++) {
				printf("%02x ", buffer[i]);
			}
		}
		printf("\n");
	}

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

int usbtenki_getRaw(USBTenki_dev_handle hdl, int id, unsigned char *dst)
{
	return usbtenki_command(hdl, USBTENKI_GET_RAW, id, dst);
}

int usbtenki_getNumChannels(USBTenki_dev_handle hdl)
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

int usbtenki_getChipID(USBTenki_dev_handle hdl, int id)
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

float usbtenki_convertFrequency(float freq, int src_fmt, int dst_fmt)
{
	double hz;

	if (src_fmt == dst_fmt)
		return freq;

	switch (src_fmt)
	{
		case TENKI_UNIT_MILLIHZ:
			hz = freq / 1000;
			break;
		case TENKI_UNIT_HZ:
			hz = freq;
			break;
		case TENKI_UNIT_KHZ:
			hz = freq * 1000;
			break;
		case TENKI_UNIT_MHZ:
			hz = freq * 1000000;
			break;
		case TENKI_UNIT_RPM:
			hz = freq / 60;
			break;
		default:
			return freq;
	}

	switch (dst_fmt)
	{
		case TENKI_UNIT_MILLIHZ:
			return hz * 1000;
		case TENKI_UNIT_HZ:
			return hz;
		case TENKI_UNIT_KHZ:
			return hz / 1000;
		case TENKI_UNIT_MHZ:
			return hz / 1000000;
		case TENKI_UNIT_RPM:
			return hz * 60;
	}

	return freq;
}

float usbtenki_convertPressure(float pressure, int src_fmt, int dst_fmt)
{
	float pascals;

	if (src_fmt == dst_fmt)
		return pressure;

	switch (src_fmt)
	{
		case TENKI_UNIT_KPA:
			pascals = pressure * 1000.0;
			break;

		case TENKI_UNIT_HPA:
			pascals = pressure * 100.0;
			break;

		case TENKI_UNIT_BAR:
			pascals = pressure * 100000.0;
			break;

		case TENKI_UNIT_AT:
			pascals = pressure * 98066.5;
			break;

		case TENKI_UNIT_ATM:
			pascals = pressure * 101325;
			break;

		case TENKI_UNIT_TORR:
			pascals = pressure * 133.322;
			break;

		case TENKI_UNIT_PSI:
			pascals = pressure * 6894.76;
			break;

		default:
			return pressure;
	}

	switch (dst_fmt)
	{
		case TENKI_UNIT_KPA: return pascals / 1000.0;
		case TENKI_UNIT_HPA: return pascals / 100.0;
		case TENKI_UNIT_BAR: return pascals / 100000.0;
		case TENKI_UNIT_AT: return pascals / 98066.5;
		case TENKI_UNIT_ATM: return pascals / 101325.0;
		case TENKI_UNIT_TORR: return pascals / 133.322;
		case TENKI_UNIT_PSI: return pascals / 6894.76;
	}

	return pressure;
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



const char *chipToString(int id)
{
	switch(id)
	{
		case USBTENKI_CHIP_ADT7410:
			return "ADT7410 I2C Temperature sensor";
		case USBTENKI_CHIP_SE95:
			return "SE95 I2C Temperature sensor";
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
		case USBTENKI_CHIP_BS02_TEMP:
			return "BS02 Temperature";
		case USBTENKI_CHIP_BS02_RH:
			return "BS02 Relative Humidity";

		case USBTENKI_CHIP_TSL2561_IR_VISIBLE:
			return "TSL2561 Channel 0 (IR+Visibile)";
		case USBTENKI_CHIP_TSL2561_IR:
			return "TSL2561 Channel 1 (IR only)";
		case USBTENKI_CHIP_TSL2561_IR_VISIBLE_16X:
			return "TSL2561 Channel 0 (IR+Visibile) 16X gain";
		case USBTENKI_CHIP_TSL2561_IR_16X:
			return "TSL2561 Channel 1 (IR only) 16X gain";

		case USBTENKI_CHIP_TSL2568_IR_VISIBLE:
			return "TSL2568 Channel 0 (IR+Visibile)";
		case USBTENKI_CHIP_TSL2568_IR:
			return "TSL2568 Channel 1 (IR only)";
		case USBTENKI_CHIP_TSL2568_IR_VISIBLE_16X:
			return "TSL2568 Channel 0 (IR+Visibile) 16X gain";
		case USBTENKI_CHIP_TSL2568_IR_16X:
			return "TSL2568 Channel 1 (IR only)";

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

		case USBTENKI_CHIP_MP3H6115A:
			return "MP3H6115A Absolute air pressure sensor";

		case USBTENKI_CHIP_VOLTS:
			return "Ratiometric volts from ADC";

		case USBTENKI_CHIP_VOLTS_REVERSE:
			return "Inverted ratiometric volts from ADC";

		case USBTENKI_CHIP_TACHOMETER:
			return "Tachometer";

		/* Virtual channels and chipID have the same vales */
		case USBTENKI_VIRTUAL_DEW_POINT:
			return "Dew point";
		case USBTENKI_VIRTUAL_HUMIDEX:
			return "Humidex";
		case USBTENKI_VIRTUAL_HEAT_INDEX:
			return "Heat index";
	
		case USBTENKI_VIRTUAL_TSL2561_LUX:
			return "TSL2561 Lux";

		 case USBTENKI_VIRTUAL_TSL2568_LUX:
		 	return "TSL2568 Lux";

		case USBTENKI_CHIP_NONE:
			return "Unused/unconfigured";

	}
	return "unkown";
}

const char *chipToShortString(int id)
{
	switch(id)
	{
		case USBTENKI_CHIP_ADT7410:
		case USBTENKI_CHIP_SE95:
		case USBTENKI_CHIP_MCP9800:
		case USBTENKI_CHIP_LM75:
		case USBTENKI_CHIP_LM92:
		case USBTENKI_CHIP_SHT_TEMP:
		case USBTENKI_CHIP_BS02_TEMP:
			return "Temperature";
		
		case USBTENKI_CHIP_TSL2561_IR_VISIBLE:
		case USBTENKI_CHIP_TSL2568_IR_VISIBLE:
			return "Visible and IR";

		case USBTENKI_CHIP_TSL2561_IR_VISIBLE_16X:
		case USBTENKI_CHIP_TSL2568_IR_VISIBLE_16X:
			return "Visible and IR (16x gain)";

		case USBTENKI_CHIP_TSL2561_IR:
		case USBTENKI_CHIP_TSL2568_IR:
			return "IR";

		case USBTENKI_CHIP_TSL2561_IR_16X:
		case USBTENKI_CHIP_TSL2568_IR_16X:
			return "IR (16x gain)";			

		case USBTENKI_CHIP_SHT_RH:
		case USBTENKI_CHIP_BS02_RH:
			return "Relative Humidity";

		case USBTENKI_MCU_ADC0:
		case USBTENKI_MCU_ADC1:
		case USBTENKI_MCU_ADC2:
		case USBTENKI_MCU_ADC3:
		case USBTENKI_MCU_ADC4:
		case USBTENKI_MCU_ADC5:
			return "Raw ADC output";

		case USBTENKI_CHIP_MPX4115:
		case USBTENKI_CHIP_MP3H6115A:
			return "Pressure";

		case USBTENKI_CHIP_VOLTS_REVERSE:
		case USBTENKI_CHIP_VOLTS:
			return "Voltage";

		case USBTENKI_CHIP_D6F_V03A1:
			return "Air speed";

		case USBTENKI_CHIP_TACHOMETER:
			return "Frequency";

		/* Virtual channels and chipID share the same namespace */
		case USBTENKI_VIRTUAL_DEW_POINT:
			return "Dew point";
		case USBTENKI_VIRTUAL_HUMIDEX:
			return "Humidex";
		case USBTENKI_VIRTUAL_HEAT_INDEX:
			return "Heat index";
		case USBTENKI_VIRTUAL_TSL2568_LUX:
		case USBTENKI_VIRTUAL_TSL2561_LUX:
			return "Lux";

		case USBTENKI_CHIP_NONE:
			return "N/A";
	}
	return "unknown";
}

const char *unitToString(int unit, int no_fancy_chars)
{
	switch(unit)
	{
		case TENKI_UNIT_RH: return "%";

		/* Note: The degree symbol may appear incorrectly as two characters
		 * depending on your encoding here */
		case TENKI_UNIT_CELCIUS: return no_fancy_chars ? "C" : "°C";
		case TENKI_UNIT_KELVIN: return no_fancy_chars ? "K" : "°K";
		case TENKI_UNIT_FAHRENHEIT: return no_fancy_chars ? "F" : "°F";

		case TENKI_UNIT_RAW: return "(raw)";
		case TENKI_UNIT_KPA: return "kPa";
		case TENKI_UNIT_HPA: return "hPa";
		case TENKI_UNIT_BAR: return "bar";
		case TENKI_UNIT_AT: return "at";
		case TENKI_UNIT_ATM: return "atm";
		case TENKI_UNIT_TORR: return "Torr";
		case TENKI_UNIT_PSI: return "psi";
		case TENKI_UNIT_VOLTS: return "V";
		case TENKI_UNIT_LUX: return "lx";
		case TENKI_UNIT_METER_SEC: return "m/sec";
		case TENKI_UNIT_MILLIHZ: return "mHz";
		case TENKI_UNIT_HZ: return "Hz";
		case TENKI_UNIT_KHZ: return "kHz";
		case TENKI_UNIT_MHZ: return "MHz";
		case TENKI_UNIT_RPM: return "rpm";
	}

	return "";
}

int usbtenki_readChannel(USBTenki_dev_handle hdl, struct USBTenki_channel *chn)
{
	return usbtenki_readChannelList(hdl, &chn->channel_id, 1, chn, 1, 1);
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
int usbtenki_readChannelList(USBTenki_dev_handle hdl, int *channel_ids, int num, struct USBTenki_channel *dst, int dst_total, int num_attempts)
{
	int i, j, res;
	int n;

	for (i=0; i<num; i++)
	{
		// skip virtual channels
		if (channel_ids[i]>=USBTENKI_VIRTUAL_START)
			continue;

		// Look for the destination
		for (j=0; j<dst_total; j++) {
			if (channel_ids[i] == dst[j].channel_id) {
				break;
			}
		}
		if (j==dst_total) {
			fprintf(stderr, "Invalid channel ID (%d) requested\n", channel_ids[i]);
			continue;
		}

		if (dst[j].data_valid)
			continue; /* already done */

		for (n=0; n<num_attempts; n++) {
			dst[j].raw_length = usbtenki_getRaw(hdl, dst[j].channel_id, dst[j].raw_data);
			if (dst[j].raw_length<0) {
				usleep(200);
				continue;
			}
			break;
		}
		
		/* all attempts failed? */
		if (n==num_attempts) {
			return -1;
		}
		
		dst[j].data_valid = 1;
		res = usbtenki_convertRaw(&dst[j]);
		if (res==-1) {
			fprintf(stderr, "Failed to convert raw value from chip %d, channel: %d\n",
								dst[j].chip_id, dst[j].channel_id);
		}
	}

	return 0;
}

/**
 * \brief Populate an array of struct USBTEnki_channel from a device
 * \return The number of channels
 * This does not read the channels.
 */
int usbtenki_listChannels(USBTenki_dev_handle hdl, struct USBTenki_channel *dstArray, int arr_size)
{
	int n_channels;
	int i;

	n_channels = usbtenki_getNumChannels(hdl);
	for (i=0; i<n_channels && i<arr_size; i++){
		memset(dstArray, 0, sizeof(struct USBTenki_channel));
		dstArray->channel_id = i;
		dstArray->chip_id = usbtenki_getChipID(hdl, i);
		dstArray->data_valid = 0;

		dstArray++;
	}

	if (n_channels > arr_size)
		fprintf(stderr, "warning: Channel list truncated\n");

	return n_channels;
}

/**
 * \brief Add a virtual channel to a list of channels.
 * \param channels The channel list
 * \param num_channels Pointer to an integer representing the number of channel currently in list.
 * \param max_channels The maximum number of channels that can be present in 'channels'
 * \param channel The channel to add.
 */
static int addVirtualChannel(struct USBTenki_channel *channels, int *num_channels, 
						int max_channels, struct USBTenki_channel *channel)
{
	if (*num_channels >= max_channels) {
		fprintf(stderr, "warning: Not enough space for all virtual channels\n");
		return -1;
	}

	if (g_usbtenki_verbose)
		printf("Adding channel to index %d\n", *num_channels);

	memcpy(&channels[*num_channels], channel, sizeof(struct USBTenki_channel));
	(*num_channels)++;

	return 0;
}

/**
 * \brief Search the list of channels for the channel_id corresponding to a specific chip id.
 */
static int chipIdToChannelId(struct USBTenki_channel *channels, int num_channels, int chip_id)
{
	int i;
	for (i=0; i<num_channels; i++)
	{
		if (channels[i].chip_id == chip_id) {
			if (g_usbtenki_verbose)
				printf("Chip %d is at channel %d\n", chip_id,
					channels[i].channel_id);
			return channels[i].channel_id;
		}
	}
	return -1;
}

/**
 * Returns a pointer to a specific channel_id from a list of channels, optionally
 * reading data from the device if the channel's data was not yet valid.
 * */
static struct USBTenki_channel *getValidChannel(USBTenki_dev_handle hdl, struct USBTenki_channel *channels, int num_channels, int requested_channel_id)
{
	int i, res;	

	for (i=0; i<num_channels; i++)
	{
		if (channels[i].channel_id == requested_channel_id)
		{
			if (g_usbtenki_verbose)
				printf("%s: found channel id %d at index %d\n" , __FUNCTION__, 
							requested_channel_id, i);

			if (channels[i].data_valid) {
				if (g_usbtenki_verbose) 
					printf("Data already valid for this channel.\n");
				return &channels[i];
			}				

			res = usbtenki_readChannelList(hdl, &requested_channel_id, 1, channels, num_channels, g_usbtenki_num_attempts);
			if (res!=0) {
				fprintf(stderr, "Failed to read channel %d data from device! (%d)\n",
					requested_channel_id, res);
				return NULL;
			}

			if (channels[i].data_valid) {
				if (g_usbtenki_verbose) 
					printf("Data is now valid for this channel.\n");
				return &channels[i];
			}				

		}
	}
	return NULL;
}


static struct USBTenki_channel *getValidChannelFromChip(USBTenki_dev_handle hdl, struct USBTenki_channel *channels, int num_channels, int requested_chip_id)
{
	int channel_id;

	channel_id = chipIdToChannelId(channels, num_channels, requested_chip_id);
	if (channel_id < 0) 
		return NULL;
	return getValidChannel(hdl, channels, num_channels, channel_id);
}

int usbtenki_processVirtualChannels(USBTenki_dev_handle hdl, struct USBTenki_channel *channels, int num_channels)
{
	int i;
	struct USBTenki_channel *chn;

	for (i=0; i<num_channels; i++)
	{
		chn = &channels[i];

		if (chn->channel_id < USBTENKI_VIRTUAL_START)
			continue;

		switch(chn->channel_id)
			{
				case USBTENKI_VIRTUAL_TSL2568_LUX:
					{
						struct USBTenki_channel *vir_chn, *ir_chn;
						double ch0,ch1,lx;

						ir_chn = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2568_IR);

						vir_chn = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2568_IR_VISIBLE);

						if (ir_chn==NULL || vir_chn==NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						ch0 = vir_chn->converted_data;
						ch1 = ir_chn->converted_data;

						if ((vir_chn->converted_data < 3000) && (ir_chn->converted_data < 3000)) {
							struct USBTenki_channel *vir_chn_g, *ir_chn_g;
							double ch0_g, ch1_g;

							/* Based on these values, a 16x gain would not overflow. */
//							printf("Switching to 16x gain\n");
							ir_chn_g = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2568_IR_16X);
	
							vir_chn_g = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2568_IR_VISIBLE_16X);

						
							ch0_g = vir_chn_g->converted_data;
							ch1_g = ir_chn_g->converted_data;
	
//							printf("%.f %.f %.f %.f\n", ch0, ch0_g/16.0, ch1, ch1_g/16.0);
				
							if (ir_chn_g != NULL && vir_chn_g != NULL) {
								// stick to the low gain channels in case of saturation
								if (ir_chn_g->converted_data != 65535 && 
									vir_chn_g->converted_data != 65535) {
									ch0 = ch0_g / 16.0;
									ch1 = ch1_g / 16.0;
								}
							}
						}
						
						if (ch0 > 65534 || ch1 > 65534) {	
							chn->data_valid = 0;
							chn->saturated = 1;
							chn->converted_data = -1;
							chn->converted_unit = TENKI_UNIT_LUX;
							break;
						}

						/*						 
						TMB Package
							For 0 < CH1/CH0 < 0.35     Lux = 0.00763  CH0 - 0.01031  CH1
							For 0.35 < CH1/CH0 < 0.50  Lux = 0.00817  CH0 - 0.01188  CH1
							For 0.50 < CH1/CH0 < 0.60  Lux = 0.00723  CH0 - 0.01000  CH1
							For 0.60 < CH1/CH0 < 0.72  Lux = 0.00573  CH0 - 0.00750  CH1
							For 0.72 < CH1/CH0 < 0.85  Lux = 0.00216  CH0 - 0.00254  CH1
							For CH1/CH0 > 0.85         Lux = 0
						*/
						if (ch1/ch0 < 0.35)
							lx = 0.00763 * ch0 - 0.01031 * ch1;
						else if (ch1/ch0 < 0.50)
							lx = 0.00817 * ch0 - 0.01188 * ch1;
						else if (ch1/ch0 < 0.60)
							lx = 0.00723 * ch0 - 0.01000 * ch1;
						else if (ch1/ch0 < 0.72)
							lx = 0.00573 * ch0 - 0.00750 * ch1;
						else if (ch1/ch0 < 0.85)
							lx = 0.00216 * ch0 - 0.00254 * ch1;
						else
							lx = 0.0;
						
//						printf("ch1: %f, ch0: %f, lx: %f\n", ch1, ch0, lx);

						chn->data_valid = 1;
						chn->converted_data = lx;
						chn->converted_unit = TENKI_UNIT_LUX;
					}
					break;


				case USBTENKI_VIRTUAL_TSL2561_LUX:
					{
						struct USBTenki_channel *vir_chn, *ir_chn;
						float ch0,ch1,lx;

						ir_chn = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2561_IR);

						vir_chn = getValidChannelFromChip(hdl, channels, num_channels,
														USBTENKI_CHIP_TSL2561_IR_VISIBLE);

						if (ir_chn==NULL || vir_chn==NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						ch0 = vir_chn->converted_data;
						ch1 = ir_chn->converted_data;
						
						/*
						 * TMB Package
						 *
						 * For 0 < CH1/CH0 <= 0.50 		Lux = 0.0304 * CH0 - .062 * CH0 * ((CH1/CH0)1.4)
						 * For 0.50 < CH1/CH0 <=  0.61 	Lux = 0.0224 * CH0 - .031 * CH1
						 * For 0.61 < CH1/CH0 <= 0.80 	Lux = 0.0128 * CH0 - .0153 * CH1
						 * For 0.80 < CH1/CH0 <= 1.30 	Lux = 0.00146 *  CH0 - .00112 * CH1
						 * For CH1/CH0 > 1.30			Lux = 0
						 *
						 */
						if (ch1/ch0 < 0.50)
							lx = 0.0304 * ch0 - 0.062 * ch0 * (pow((ch1/ch0),1.4));
						else if (ch1/ch0 < 0.61)
							lx = 0.0224 * ch0 - 0.031 * ch1;
						else if (ch1/ch0 < 0.80)
							lx = 0.0128 * ch0 - 0.0153 * ch1;
						else
							lx = 0.0;
						
//						printf("ch1: %f, ch0: %f, lx: %f\n", ch1, ch0, lx);

						chn->data_valid = 1;
						chn->converted_data = lx;
						chn->converted_unit = TENKI_UNIT_LUX;
					}
					break;
				
				case USBTENKI_VIRTUAL_DEW_POINT:
					{
						struct USBTenki_channel *temp_chn, *rh_chn;
						float H, Dp, T;

						if (g_usbtenki_verbose)
							printf("Processing dew point virtual channel\n");

						
						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_TEMP);
						if (!temp_chn) {
							temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_TEMP);
						}

						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_RH);
						if (!rh_chn) {
							rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_RH);
						}

						if (temp_chn == NULL || rh_chn == NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						T = temp_chn->converted_data;
						H = (log10(rh_chn->converted_data)-2.0)/0.4343 + 
							(17.62*T)/(243.12+T);
						Dp = 243.12 * H / (17.62 - H);

						chn->data_valid = 1;
						chn->converted_data = Dp;
						chn->converted_unit = TENKI_UNIT_CELCIUS;
					}
					break;

				case USBTENKI_VIRTUAL_HUMIDEX:
					{
						struct USBTenki_channel *temp_chn, *rh_chn;
						float H, Dp, T, h, e;
						int non_significant = 0;

						if (g_usbtenki_verbose)
							printf("Processing humidex virtual channel\n");

						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_TEMP);
						if (!temp_chn) {
							temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_TEMP);
						}

						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_RH);
						if (!rh_chn) {
							rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_RH);
						}
	
						if (temp_chn == NULL || rh_chn == NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						T = temp_chn->converted_data;
						H = (log10(rh_chn->converted_data)-2.0)/0.4343 + (17.62*T)/(243.12+T);
						Dp = 243.12 * H / (17.62 - H);
	
						if (Dp < 0) {
							// Weatheroffice.gc.ca: We only display humidex values of 25 or higher for a 
							// location which reports a dew point temperature above zero (0°C) ...
							non_significant = 1;
						}
						if (T < 20) {
							// ... AND an air temperature of 20°C or more.
							//
							non_significant = 1;
						}
		
						/* We need dewpoint in kelvins... */
						Dp = usbtenki_convertTemperature(Dp, TENKI_UNIT_CELCIUS, TENKI_UNIT_KELVIN);
		
						e = 6.11 * exp(5417.7530 * ((1.0/273.16) - (1.0/Dp)));
						h = (5.0/9.0)*(e - 10.0);
						
						if (!non_significant) {
							chn->converted_data = T + h;
							chn->converted_unit = TENKI_UNIT_CELCIUS;
						} else {
							chn->converted_data = T;
							chn->converted_unit = TENKI_UNIT_CELCIUS;
						}
						
						chn->data_valid = 1;
					}
					break;

				case USBTENKI_VIRTUAL_HEAT_INDEX:
					{
						struct USBTenki_channel *temp_chn, *rh_chn;
						float T, R, HI;
						int out_of_range = 0;

						if (g_usbtenki_verbose)
							printf("Processing heat index virtual channel\n");

						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_TEMP);
						if (!temp_chn) {
							temp_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_TEMP);
						}

						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_SHT_RH);
						if (!rh_chn) {
							rh_chn = getValidChannelFromChip(hdl, channels, num_channels, USBTENKI_CHIP_BS02_RH);
						}
	
						if (temp_chn == NULL || rh_chn == NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						T = temp_chn->converted_data;
						T =  usbtenki_convertTemperature(T, TENKI_UNIT_CELCIUS, 
															TENKI_UNIT_FAHRENHEIT);
						R = rh_chn->converted_data;
		
						/* Formula source: 
						 * Initially: http://www.crh.noaa.gov/jkl/?n=heat_index_calculator (2012: 404 error)
						 *
						 * http://en.wikipedia.org/wiki/Heat_index#Formula
						 */
						HI = 	-42.379 + 
								2.04901523 * T + 
								10.14333127 * R - 
								0.22475541 * T * R - 
								6.83783 * pow(10,-3) * pow(T, 2) - 
								5.481717 * pow(10,-2) * pow(R, 2) + 
								1.22874 * pow(10,-3) * pow(T, 2) * R + 
								8.5282 * pow(10,-4) * T * pow(R, 2) - 
								1.99 * pow(10,-6) * pow(T,2) * pow(R,2);

			
						if (T < 80 || R < 40) {
							out_of_range = 1;
						}

						if (out_of_range) {
							chn->data_valid = 1;
							chn->converted_data = T;
							chn->converted_unit = TENKI_UNIT_FAHRENHEIT;
						} else {
							chn->data_valid = 1;
							chn->converted_data = HI;
							chn->converted_unit = TENKI_UNIT_FAHRENHEIT;
						}
					}
					break;
			}

	}

	return 0;
}

int usbtenki_addVirtualChannels(struct USBTenki_channel *channels, int *num_channels, 
															int max_channels)
{
	int i;
	struct USBTenki_channel chn;

	int real_channels = *num_channels;

	/* Dew point, humidex and heat index from Temp+RH for sensirion sht1x/7x and BS02 */
	if (1)
	{
		int hfound=0, tfound=0;
		for (i=0; i<real_channels; i++)
		{
			if (channels[i].chip_id == USBTENKI_CHIP_SHT_TEMP)
				tfound = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_SHT_RH)
				hfound = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_BS02_TEMP)
				tfound = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_BS02_RH)
				hfound = 1;

		}

		if (hfound && tfound) {
			chn.channel_id = USBTENKI_VIRTUAL_DEW_POINT;
			chn.chip_id = chn.channel_id;
			chn.data_valid = 0;
			chn.converted_data = 0.0;
			chn.converted_unit = 0;
			if (addVirtualChannel(channels, num_channels, max_channels, &chn))
				return -1;

			chn.channel_id = USBTENKI_VIRTUAL_HUMIDEX;
			chn.chip_id = chn.channel_id;
			
			if (addVirtualChannel(channels, num_channels, max_channels, &chn))
				return -1;

			chn.channel_id = USBTENKI_VIRTUAL_HEAT_INDEX;
			chn.chip_id = chn.channel_id;

			if (addVirtualChannel(channels, num_channels, max_channels, &chn))
				return -1;

		}
	}

	/* Lux calculated using Visible + IR and IR only channels from TSL2561 sensor */
	if (1)
	{
		int vir_found=0, ir_found=0;
		for (i=0; i<real_channels; i++)
		{
			if (channels[i].chip_id == USBTENKI_CHIP_TSL2561_IR_VISIBLE)
				vir_found = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_TSL2561_IR)
				ir_found = 1;
		}

		if (vir_found && ir_found) {
			chn.channel_id = USBTENKI_VIRTUAL_TSL2561_LUX;
			chn.chip_id = chn.channel_id;
			chn.data_valid = 0;
			chn.converted_data = 0.0;
			chn.converted_unit = 0;
			if (addVirtualChannel(channels, num_channels, max_channels, &chn))
				return -1;
		}
	}

	/* Lux calculated using Visible + IR and IR only channels from TSL2568 sensor */
	if (1)
	{
		int vir_found=0, ir_found=0;
		for (i=0; i<real_channels; i++)
		{
			if (channels[i].chip_id == USBTENKI_CHIP_TSL2568_IR_VISIBLE)
				vir_found = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_TSL2568_IR)
				ir_found = 1;
		}

		if (vir_found && ir_found) {
			chn.channel_id = USBTENKI_VIRTUAL_TSL2568_LUX;
			chn.chip_id = chn.channel_id;
			chn.data_valid = 0;
			chn.converted_data = 0.0;
			chn.converted_unit = 0;
			if (addVirtualChannel(channels, num_channels, max_channels, &chn))
				return -1;
		}
	}



	return 0;
}
