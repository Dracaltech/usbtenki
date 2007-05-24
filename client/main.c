/* usbtenkiget: A command-line tool for USBTenki sensors.
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

#define ARRAY_SIZE(arr) ( sizeof(arr) / sizeof(*(arr)))

#define DEFAULT_CHANNEL_ID	0
#define DEFAULT_NUM_SAMPLES 1
#define MAX_CHANNELS		256


int g_verbose = 0;
int g_temp_format = TENKI_UNIT_CELCIUS;
int g_pressure_format = TENKI_UNIT_KPA;
int g_pretty = 0;
int g_full_display_mode = 0;

int processChannels(usb_dev_handle *hdl, int *requested_channels, int num_req_chns);
int addVirtualChannels(struct USBTenki_channel *channels, int *num_channels,
																	int max_channels);

static void printUsage(void)
{
	printf("Usage: ./usbtenkiget [options]\n");
	printf("\nValid options:\n");
	printf("    -V          Display version information\n");
	printf("    -v          Verbose mode\n");
	printf("    -h          Displays help\n");
	printf("    -l          List and display info about available sensors\n");
	printf("    -f          Full list mode. (shows unused/unconfigured channels)\n");
	printf("    -s serno    Use USB sensor with matching serial number. Default: Use first.\n");
	printf("    -i id<,id,id...>  Use specific channel(s) id(s) or 'a' for all. Default: %d\n", DEFAULT_CHANNEL_ID);

	printf("    -T unit     Select the temperature unit to use. Default: Celcius\n");
	printf("    -P unit     Select the pressure unit to use. Default: kPa\n");

	printf("    -p          Enable pretty output\n");

	printf("\nValid temperature units:\n");
	printf("    Celcius, c, Fahrenheit, f, Kelvin, k\n");
	printf("\nValid pressure units:\n");
	printf("    kPa, hPa, bar, at (98.0665 kPa), atm (101.325 kPa), Torr, psi\n");
}

static void printVersion(void)
{
	printf("USBTenkiget version %s, Copyright (C) 2007, Raphael Assenat\n\n", VERSION);
	printf("This software comes with ABSOLUTELY NO WARRANTY;\n");
	printf("You may redistribute copies of it under the terms of the GNU General Public License\n");
	printf("http://www.gnu.org/licenses/gpl.html\n");
}


int main(int argc, char **argv)
{
	usb_dev_handle *hdl;
	struct usb_device *cur_dev, *dev=NULL;
	int res, i;
	struct USBTenki_list_ctx rgblistctx;
	struct USBTenki_info info;

	char *use_serial = NULL;
	int list_mode = 0;

	int requested_channels[MAX_CHANNELS];
	int num_requested_channels= 1;
	int use_all_channels = 0;
	

	requested_channels[0] = DEFAULT_CHANNEL_ID;

	while (-1 != (res=getopt(argc, argv, "Vvhlfs:i:T:P:p")))
	{
		switch (res)
		{
			case 'V':
				printVersion();
				return 0;
			case 'v':
				g_verbose = 1;
				break;
			case 'h':
				printUsage();
				return 0;			
			case 'l':
				list_mode = 1;
				break;			
			case 'f':
				g_full_display_mode = 1;
				break;			
			case 's':
				use_serial = optarg;	
				break;			
			case 'i':
				{
					char *p;
					char *e;
	
					if (*optarg == 'a') 
					{
						num_requested_channels = 0;
						use_all_channels = 1;
						break;
					}


					num_requested_channels = 0;
					p = optarg;
					while(1) {
						if (num_requested_channels >= MAX_CHANNELS) {
							fprintf(stderr,"too many channels\n");
							return -1;
						}

						requested_channels[num_requested_channels] = strtol(p, &e, 0);
						if (e==p) {
							fprintf(stderr, "Error in channel list\n");
							return -1;
						}
						
						num_requested_channels++;
						
						if (*e==0)
							break;
					
						if (*e==',') {
							e++;
						}
						else {
							fprintf(stderr, "Error in channel list\n");
							return -1;
						}

						p = e;
					}
				}
				break;
			
			case 'T':
				if (strcasecmp(optarg, "Celcius")==0 || 
						strcasecmp(optarg, "C")==0)
					g_temp_format = TENKI_UNIT_CELCIUS;
				else if (strcasecmp(optarg, "Fahrenheit")==0 ||
						strcasecmp(optarg, "F")==0)
					g_temp_format = TENKI_UNIT_FAHRENHEIT;
				else if (strcasecmp(optarg, "Kelvin")==0 ||
						strcasecmp(optarg, "K")==0)
					g_temp_format = TENKI_UNIT_KELVIN;
				else {
					fprintf(stderr, "Unknown temperature format: '%s'\n",
										optarg);
					return -1;
				}					
				break;
	
			case 'P':
				{
					struct {
						const char *name;
						int fmt;
					} tbl[] = { 
						{ "kpa", TENKI_UNIT_KPA },
						{ "hpa", TENKI_UNIT_HPA },
						{ "bar", TENKI_UNIT_BAR },
						{ "at", TENKI_UNIT_AT },
						{ "atm", TENKI_UNIT_ATM },
						{ "torr", TENKI_UNIT_TORR },
						{ "psi", TENKI_UNIT_PSI },
					};
					
					for (i=0; i<ARRAY_SIZE(tbl); i++) {
						if (strcasecmp(tbl[i].name, optarg)==0) {
							g_pressure_format = tbl[i].fmt;
							break;
						}
					}
					if (i==ARRAY_SIZE(tbl)) {
						fprintf(stderr, 
							"Unknown pressure format: '%s'\n", optarg);
					}
				}
				break;

			case 'p':
				g_pretty = 1;
				break;

			case '?':
				fprintf(stderr, "Unknown argument specified\n");
				return -1;
		}
	}

	if (g_verbose) {
		printf("Arguments {\n");
		printf("  verbose: yes\n");
		printf("  use_all_channels: %d\n", use_all_channels);
		printf("  channel id(s): ");
		for (i=0; i<num_requested_channels; i++) {
			printf("%d ", requested_channels[i]);
		}
		printf("\n");
		printf("  g_temp_format: %d\n", g_temp_format);
		printf("  list_mode: %d\n", list_mode);
		if (use_serial)
			printf("  use_serial: %s\n", use_serial);
		printf("}\n");
	}

	usb_init();
	usb_find_busses();
	usb_find_devices();

	usbtenki_initListCtx(&rgblistctx);

	while ((cur_dev=usbtenki_listDevices(&info, &rgblistctx)))
	{
		if (use_serial) {
			if (strcmp(use_serial, info.str_serial)==0) {
				dev = cur_dev;
			}
			else
				continue; // ignore other serials
		}
		else {
			dev = cur_dev; // Last found will be used
		}

		if (list_mode || g_verbose) {
			struct USBTenki_channel tmpchannels[MAX_CHANNELS];
			int n_channels, i;

			printf("Found: '%s', ", info.str_prodname);
			printf("Serial: '%s', ", info.str_serial);
			printf("Version %d.%d, ", info.major,
									info.minor);
		
			if (list_mode)
			{
				hdl = usb_open(dev);
				if (!hdl) {
					printf("\n");
					printf("    Error, cannot open device (%s)\n", usb_strerror());
					continue;
				}


				n_channels = usbtenki_listChannels(hdl, tmpchannels, MAX_CHANNELS);
				if (n_channels==-1) {
					continue;
				}

				addVirtualChannels(tmpchannels, &n_channels, MAX_CHANNELS);
				printf("Channels: %d\n", n_channels);
				for (i=0; i<n_channels; i++) {
					if (tmpchannels[i].channel_id >= USBTENKI_VIRTUAL_START) {
						printf("    Virtual Channel %d: %s [%s]\n",
								tmpchannels[i].channel_id,
								chipToString(tmpchannels[i].chip_id),
								chipToShortString(tmpchannels[i].chip_id));
					}
					else {
						if (!g_full_display_mode && 
							tmpchannels[i].chip_id == USBTENKI_CHIP_NONE)
							continue;

						printf("    Channel %d: %s [%s]\n",
								tmpchannels[i].channel_id,
								chipToString(tmpchannels[i].chip_id),
								chipToShortString(tmpchannels[i].chip_id));
					}
				}

				usb_close(hdl);
			}
		}
	}

	if (!dev) {
		if (use_serial)
			printf("Device with serial '%s' not found\n", use_serial);
		else
			printf("No device found\n");
		return 1;
	}

	if (list_mode) {
		return 0;
	}

	hdl = usb_open(dev);
	if (!hdl) {
		printf("USB Error: %s\n", usb_strerror());
		return 1;
	}

	if (g_verbose)
		printf("Claiming interface\n");

	res = usb_claim_interface(hdl, 0);
	if (res<0) {
		printf("USB Error (usb_claim_interface: %s)\n", usb_strerror());
		return 2;
	}	
	
	processChannels(hdl, requested_channels, num_requested_channels);

	usb_release_interface(hdl, 0);
	usb_close(hdl);

	return res;
}

/**
 * \brief Add a virtual channel to a list of channels.
 * \param channels The channel list
 * \param num_channels Pointer to an integer representing the number of channel currently in list.
 * \param max_channels The maximum number of channels that can be present in 'channels'
 * \param channel The channel to add.
 */
int addVirtualChannel(struct USBTenki_channel *channels, int *num_channels, 
						int max_channels, struct USBTenki_channel *channel)
{
	if (*num_channels >= max_channels) {
		fprintf(stderr, "warning: Not enough space for all virtual channels\n");
		return -1;
	}

	if (g_verbose)
		printf("Adding channel to index %d\n", *num_channels);

	memcpy(&channels[*num_channels], channel, sizeof(struct USBTenki_channel));
	(*num_channels)++;

	return 0;
}

int addVirtualChannels(struct USBTenki_channel *channels, int *num_channels, 
															int max_channels)
{
	int i;
	struct USBTenki_channel chn;

	int real_channels = *num_channels;

	/* Dew point, humidex and heat index from Temp+RH for sensirion sht1x/7x */
	if (1)
	{
		int hfound=0, tfound=0;
		for (i=0; i<real_channels; i++)
		{
			if (channels[i].chip_id == USBTENKI_CHIP_SHT_TEMP)
				tfound = 1;
			if (channels[i].chip_id == USBTENKI_CHIP_SHT_RH)
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
			if (g_verbose)
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
static struct USBTenki_channel *getValidChannel(usb_dev_handle *hdl, struct USBTenki_channel *channels, int num_channels, int requested_channel_id)
{
	int i, res;	

	for (i=0; i<num_channels; i++)
	{
		if (channels[i].channel_id == requested_channel_id)
		{
			if (g_verbose)
				printf("%s: found channel id %d at index %d\n" , __FUNCTION__, 
							requested_channel_id, i);

			if (channels[i].data_valid) {
				if (g_verbose) 
					printf("Data already valid for this channel.\n");
				return &channels[i];
			}				

			res = usbtenki_readChannelList(hdl, &requested_channel_id, 1, channels, num_channels);
			if (res!=0) {
				fprintf(stderr, "Failed to read channel %d data from device! (%d)\n",
					requested_channel_id, res);
				return NULL;
			}

			if (channels[i].data_valid) {
				if (g_verbose) 
					printf("Data is now valid for this channel.\n");
				return &channels[i];
			}				

		}
	}
	return NULL;
}

static struct USBTenki_channel *getValidChannelFromChip(usb_dev_handle *hdl, struct USBTenki_channel *channels, int num_channels, int requested_chip_id)
{
	int channel_id;

	channel_id = chipIdToChannelId(channels, num_channels, requested_chip_id);
	if (channel_id < 0) 
		return NULL;
	return getValidChannel(hdl, channels, num_channels, channel_id);
}

int processVirtualChannels(usb_dev_handle *hdl, struct USBTenki_channel *channels, 
							int num_channels, int *reqst_chns, int num_req_chns)
{
	int i;
	struct USBTenki_channel *chn;
	int j;

	for (i=0; i<num_channels; i++)
	{
		chn = &channels[i];

		if (chn->channel_id >= USBTENKI_VIRTUAL_START)
		{
			// Check if this virtual channel was requested
			for (j=0; j<num_req_chns; j++)
			{
				if (reqst_chns[j]==chn->channel_id)
					break;
			}
			if (j==num_req_chns) {
				/* not requested! */
				continue;
			}
	

			switch(chn->channel_id)
			{
				case USBTENKI_VIRTUAL_DEW_POINT:
					{
						struct USBTenki_channel *temp_chn, *rh_chn;
						float H, Dp, T;

						if (g_verbose)
							printf("Processing dew point virtual channel\n");

						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																USBTENKI_CHIP_SHT_TEMP);
						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																	USBTENKI_CHIP_SHT_RH);
	
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

						if (g_verbose)
							printf("Processing dew point virtual channel\n");

						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																USBTENKI_CHIP_SHT_TEMP);
						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																	USBTENKI_CHIP_SHT_RH);
	
						if (temp_chn == NULL || rh_chn == NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						T = temp_chn->converted_data;
						H = (log10(rh_chn->converted_data)-2.0)/0.4343 + 
							(17.62*T)/(243.12+T);
						Dp = 243.12 * H / (17.62 - H);
		
						/* We need dewpoint in kelvins... */
						Dp = usbtenki_convertTemperature(Dp, TENKI_UNIT_CELCIUS, TENKI_UNIT_KELVIN);
		
						e = 6.11 * exp(5417.7530 * ((1.0/273.16) - (1.0/Dp)));
						h = (5.0/9.0)*(e - 10.0);

						chn->data_valid = 1;
						chn->converted_data = T + h;
						chn->converted_unit = TENKI_UNIT_CELCIUS;
					}
					break;

				case USBTENKI_VIRTUAL_HEAT_INDEX:
					{
						struct USBTenki_channel *temp_chn, *rh_chn;
						float T, R, HI;

						if (g_verbose)
							printf("Processing dew point virtual channel\n");

						temp_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																USBTENKI_CHIP_SHT_TEMP);
						rh_chn = getValidChannelFromChip(hdl, channels, num_channels, 
																	USBTENKI_CHIP_SHT_RH);
	
						if (temp_chn == NULL || rh_chn == NULL) {
							fprintf(stderr, "Failed to read channels required for computing virtual channel!\n");
							return -1;
						}

						T = temp_chn->converted_data;
						T =  usbtenki_convertTemperature(T, TENKI_UNIT_CELCIUS, 
															TENKI_UNIT_FAHRENHEIT);
						R = rh_chn->converted_data;
		
						/* Formula source: 
						 * http://www.crh.noaa.gov/jkl/?n=heat_index_calculator */
						HI = 	-42.379 + 
								2.04901523 * T + 
								10.14333127 * R - 
								0.22475541 * T * R - 
								6.83783 * pow(10,-3) * pow(T, 2) - 
								5.481717 * pow(10,-2) * pow(R, 2) + 
								1.22874 * pow(10,-3) * pow(T, 2) * R + 
								8.5282 * pow(10,-4) * T * pow(R, 2) - 
								1.99 * pow(10,-6) * pow(T,2) * pow(R,2);

						chn->data_valid = 1;
						chn->converted_data = HI;
						chn->converted_unit = TENKI_UNIT_FAHRENHEIT;
					}
					break;
			}
		}
	}
	
	return 0;
}

int processChannels(usb_dev_handle *hdl, int *requested_channels, int num_req_chns)
{
	struct USBTenki_channel channels[MAX_CHANNELS];
	int num_channels=0;
	int res, i;

	/* Add channels supported by the device to the array */
	res = usbtenki_listChannels(hdl, channels, MAX_CHANNELS);
	if (res<0) {
		return -1;
	}
	num_channels = res;
	if (g_verbose)
		printf("Device has a total of %d channels\n", num_channels);

	/* Add virtual channels to the list, depending on the real
	 * channels already present. (eg: Dew point and humidex can
	 * only be calculated with temperature and humidity readings. */
	addVirtualChannels(channels, &num_channels, MAX_CHANNELS);

	/* When user request all channels, num_req_chns is 0. Generate a 
	 * list of requested channels using all available real and
	 * virtual channels */
	if (!num_req_chns) {
		for (i=0; i<num_channels; i++) {

			if (!g_full_display_mode && 
					channels[i].chip_id == USBTENKI_CHIP_NONE)
				continue; // skip unused channels unless in full list

			requested_channels[num_req_chns] = channels[i].channel_id;
			num_req_chns++;
		}
	}

	/* Check if the channels requested channel(s) are available */
	for (i=0; i<num_req_chns; i++)
	{
		 int j;
		 for (j=0; j<num_channels; j++) {
		 	if (channels[j].channel_id == requested_channels[i])
				break;
		 }
		 if (j==num_channels) {
		 	fprintf(stderr, "Requested channel %d does not exist.\n", 
								requested_channels[i]);
			return -1;
		 }
	}

	/* Read all requested, real channels */
	res = usbtenki_readChannelList(hdl, requested_channels, num_req_chns, 
													channels, num_channels);
	if (res<0) {
		return -1;
	}

	if (g_verbose)
		printf("%d requested channels read successfully\n", num_req_chns);


	/* Compute virtual channels. The may read additional non-user requeted
	 * data from the device. */
	processVirtualChannels(hdl, channels, num_channels, requested_channels, num_req_chns);

	for (i=0; i<num_req_chns; i++) {
		int j;
		struct USBTenki_channel *chn;

		/* find the requested channel */
		chn = NULL;
		for (j=0; j<num_channels; j++) {
			chn = &channels[j];
			if (chn->channel_id == requested_channels[i]) 
				break;
		}

		if (!chn || !chn->data_valid) {
			fprintf(stderr, "Internal error..\n");
			res = -2;
			return -2;
		}

		/* Perform format conversion */
		switch (chn->converted_unit)
		{
			case TENKI_UNIT_FAHRENHEIT:
			case TENKI_UNIT_CELCIUS:
			case TENKI_UNIT_KELVIN:
				chn->converted_data = usbtenki_convertTemperature(chn->converted_data, 
																	chn->converted_unit,
																			g_temp_format);
				chn->converted_unit = g_temp_format;
				break;

			case TENKI_UNIT_KPA:
			case TENKI_UNIT_HPA:
			case TENKI_UNIT_BAR:
			case TENKI_UNIT_AT:
			case TENKI_UNIT_ATM:
			case TENKI_UNIT_TORR:
			case TENKI_UNIT_PSI:
				chn->converted_data = usbtenki_convertPressure(chn->converted_data, 
																chn->converted_unit,
																	g_pressure_format);
				chn->converted_unit = g_pressure_format;
				break;

		}


		if (g_pretty) {
			printf("%s: %.2f %s\n", 
					chipToShortString(chn->chip_id),
					chn->converted_data, 
					unitToString(chn->converted_unit));
		}
		else {
			printf("%.2f" , chn->converted_data);
			if (i<num_req_chns-1)
				printf(", ");
		}
	}
	if (!g_pretty)
		printf("\n");
	
	return 0;
}

