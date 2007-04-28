#include <stdio.h>
#include <string.h>
#include <usb.h>

#include "usbtemp.h"

#define DEFAULT_CHANNEL_ID	0
#define DEFAULT_NUM_SAMPLES 1

int g_verbose = 0;

static void printUsage(void)
{
	printf("Usage: ./usbtemp [options]\n");
	printf("\nValid options:\n");
	printf("    -v          Verbose mode\n");
	printf("    -h          Displays help\n");
	printf("    -l          List available sensors\n");
	printf("    -s serno    Use USB sensor with matching serial number. Default: Use first sensor found.\n");
	printf("    -i id       Use specific channel id. Default: %d\n", DEFAULT_CHANNEL_ID);
	printf("    -c          Display temperature in celcius (default)\n");
	printf("    -f          Display temperature in Farenheit\n");
	printf("    -k          Display temperature in Kelvins\n");
	printf("    -a num      Do an average of n samples (default: %d)\n", DEFAULT_NUM_SAMPLES);
}

int main(int argc, char **argv)
{
	usb_dev_handle *hdl;
	struct usb_device *cur_dev, *dev=NULL;
	int res;
	struct USBtemp_list_ctx rgblistctx;
	struct USBtemp_info info;

	int channel_id = DEFAULT_CHANNEL_ID;
	int temp_format = TEMPFMT_CELCIUS;
	int num_samples_avg = DEFAULT_NUM_SAMPLES;
	char *use_serial = NULL;
	int list_mode = 0;

	while (-1 != (res=getopt(argc, argv, "hvlcfks:i:a:")))
	{
		switch (res)
		{
			case 'v':
				g_verbose = 1;
				break;
			case 'h':
				printUsage();
				return 0;
			case 'i':
				{
					char *e;
					channel_id = strtol(optarg, &e, 0);
					if (e==optarg || channel_id < 0) {
						fprintf(stderr, "Bad channel id\n");
						return -1;
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
		printf("  channel id: %d\n", channel_id);
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

	usbtemp_initListCtx(&rgblistctx);

	while ((cur_dev=usbtemp_listDevices(&info, &rgblistctx)))
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
				printf("    Error, cannot open device\n");
				continue;
			}

			n_channels = usbtemp_getNumChannels(hdl);
			printf("Channels: %d\n", n_channels);
			
			for (i=0; i<n_channels; i++) {
				chip_id = usbtemp_getChipID(hdl, i);
				printf("    Channel %d: %s\n", i, chipToString(chip_id));
			}
			

			usb_close(hdl);

		}
	}

	if (!dev) {
		if (use_serial)
			printf("Device with serial '%s' not found\n", use_serial);
		else
			printf("No raphnet.net's usbtemp device found\n");
		return 1;
	}

	if (list_mode) {
		return 0;
	}

	hdl = usb_open(dev);
	if (!hdl) {
		printf("Error\n");
		return 1;
	}

	if (g_verbose)
		printf("Claiming interface\n");

	res = usb_claim_interface(hdl, 0);
	if (res<0) {
		printf("USB Error (usb_claim_interface: %d)\n", res);
		return 2;
	}	


	res = usbtemp_printTemperature(hdl, channel_id, temp_format, num_samples_avg);
	if (res) {
		printf("Error (get_temp: %d)\n", res);
	}

	// TODO: Release interface, close device.

	return res;
}



