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

#include "usbtenki.h"

#define DEFAULT_CHANNEL_ID	0
#define DEFAULT_NUM_SAMPLES 1
#define MAX_CHANNELS		256

int g_verbose = 0;

static void printUsage(void)
{
	printf("Usage: ./usbtenkiget [options]\n");
	printf("\nValid options:\n");
	printf("    -V          Display version information\n");
	printf("    -v          Verbose mode\n");
	printf("    -h          Displays help\n");
	printf("    -l          List available sensors\n");
	printf("    -s serno    Use USB sensor with matching serial number. Default: Use first sensor found.\n");
	printf("    -i id<,id,id...>  Use specific channel(s) id(s). Default: %d\n", DEFAULT_CHANNEL_ID);
	printf("    -c          Display temperature in Celcius (default)\n");
	printf("    -f          Display temperature in Farenheit\n");
	printf("    -k          Display temperature in Kelvins\n");
//	printf("    -a num      Do an average of n samples (default: %d)\n", DEFAULT_NUM_SAMPLES);
}

static void printVersion(void)
{
	printf("USBTempctl version %s, Copyright (C) 2007, Raphael Assenat\n\n", VERSION);
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

	int temp_format = TEMPFMT_CELCIUS;
	int num_samples_avg = DEFAULT_NUM_SAMPLES;
	char *use_serial = NULL;
	int list_mode = 0;

	int requested_channels[MAX_CHANNELS];
	int num_requested_channels= 1;
	
	struct USBTenki_channel channels[MAX_CHANNELS];
	int num_channels=0;

	requested_channels[0] = DEFAULT_CHANNEL_ID;

	while (-1 != (res=getopt(argc, argv, "Vhvlcfks:i:a:")))
	{
		switch (res)
		{
			case 'v':
				g_verbose = 1;
				break;
			case 'V':
				printVersion();
				return 0;
			case 'h':
				printUsage();
				return 0;
			case 'i':
				{
					char *p;
					char *e;
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
			case 'a':
				{
					char *e;
					num_samples_avg = strtol(optarg, &e, 0);
					if (e==optarg || num_samples_avg <= 0) {
						fprintf(stderr, "Bad number of samples\n");
						return -1;
					}
				}
				break;			
			case 's':
				use_serial = optarg;	
				break;
			
				break;
			case 'c':
				temp_format = TEMPFMT_CELCIUS;
				break;
			case 'f':
				temp_format = TEMPFMT_FAHRENHEIT;
				break;
			case 'k':
				temp_format = TEMPFMT_KELVIN;
				break;
			case 'l':
				list_mode = 1;
				break;

			case '?':
				fprintf(stderr, "Unknown argument specified\n");
				return -1;
		}
	}

	if (g_verbose) {
		printf("Arguments {\n");
		printf("  verbose: yes\n");
		printf("  channel id(s): ");
		for (i=0; i<num_requested_channels; i++) {
			printf("%d ", requested_channels[i]);
		}
		printf("\n");
		printf("  temp_format: %d\n", temp_format);
		printf("  num_samples_avg: %d\n", num_samples_avg);
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
			int n_channels, chip_id, i;

			printf("Found: '%s', ", info.str_prodname);
			printf("Serial: '%s', ", info.str_serial);
			printf("Version %d.%d, ", info.major,
									info.minor);
		
			hdl = usb_open(dev);
			if (!hdl) {
				printf("\n");
				printf("    Error, cannot open device (%s)\n", usb_strerror());
				continue;
			}

			n_channels = usbtenki_getNumChannels(hdl);
			printf("Channels: %d\n", n_channels);
			
			for (i=0; i<n_channels; i++) {
				chip_id = usbtenki_getChipID(hdl, i);
				printf("    Channel %d: %s\n", i, chipToString(chip_id));
			}
			

			usb_close(hdl);

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


	res = usbtenki_listChannels(hdl, channels, MAX_CHANNELS);
	if (res<0) {
		goto err;
	}
	num_channels = res;
	if (g_verbose)
		printf("Device has a total of %d channels\n", num_channels);

	res = usbtenki_readChannelList(hdl, requested_channels, num_requested_channels, channels, num_channels);
	if (res<0) {
		goto err;
	}
	
	if (g_verbose)
		printf("%d requested channels read successfully\n", num_requested_channels);


	for (i=0; i<num_requested_channels; i++) {
		int j;
		struct USBTenki_channel *chn;

		chn = NULL;
		for (j=0; j<num_channels; j++) {
			chn = &channels[j];
			if (chn->channel_id == requested_channels[i]) 
				break;
		}

		if (!chn || !chn->data_valid) {
			fprintf(stderr, "Internal error");
			res = -2;
			goto err;
		}

		printf("%.2f\n" , chn->converted_data);
	}

err:
	usb_release_interface(hdl, 0);
	usb_close(hdl);

	return res;
}



