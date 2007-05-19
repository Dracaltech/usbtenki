#include <stdio.h>
#include <string.h>
#include <usb.h>
#include "usbtenki.h"
#include "../common/usbtenki_cmds.h"

int g_verbose = 0;

static void printVersion(void)
{
	printf("Usbtenkisetup version %s, Copyright (C) 2007, Raphael Assenat\n\n", VERSION);
	printf("This software comes with ABSOLUTELY NO WARRANTY;\n");
	printf("You may redistribute copies of it under the terms of the GNU General Public License\n");
	printf("http://www.gnu.org/licenses/gpl.html\n");

}

static void printUsage(void)
{
	printf("Usage: ./usbtenkisetup [options] command value\n");
	printf("\nValid options:\n");
	printf("    -V          Display version information\n");
	printf("    -h          Displays help\n");
	printf("    -s serno    Use USB sensor with matching serial number. Required.\n");
	printf("\nValid commands:\n");
	printf("    setadcchip  adc_id chip\n");
	printf("    setserial   serial (8 characters)\n");

}

#define MAX_EXTRA_ARGS	8

int main(int argc, char **argv)
{
	int res, i;
	char *use_serial = NULL;
	int n_extra_args=0;
	char *eargv[MAX_EXTRA_ARGS];
	usb_dev_handle *hdl = NULL;
	struct usb_device *cur_dev, *dev=NULL;
	struct USBTenki_list_ctx rgblistctx;
	struct USBTenki_info info;
	unsigned char repBuf[8];
	int retval = 0;

	while (-1 != (res=getopt(argc, argv, "Vvhs:")))
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
			case 's':
				use_serial = optarg;
				break;
		}
	}

	n_extra_args = argc-optind;
	if (!use_serial) {
		fprintf(stderr, "Serial number is required.\n");
		return 1;
	}

	if (g_verbose) 
		printf("Extra args: %d\n", n_extra_args);

	for (i=optind; i<argc; i++) {
		eargv[i-optind] = argv[i];
		if (g_verbose) 
			printf("  %d: %s\n", i-optind, eargv[i-optind]);
	}


	usb_init();
	usb_find_busses();
	usb_find_devices();

	usbtenki_initListCtx(&rgblistctx);

	while ((cur_dev=usbtenki_listDevices(&info, &rgblistctx))) {
		if (strcmp(use_serial, info.str_serial)==0) {
			dev = cur_dev;
			break;
		}
	}
	if (!dev) {
		fprintf(stderr, "Could not locate device with serial '%s'. Try usbtekiget -l\n",
							use_serial);
		return 1;
	}

	if (n_extra_args<1) {
		fprintf(stderr, "No command specified\n");
		return 1;
	}

	hdl = usb_open(dev);
	if (!hdl) {
		fprintf(stderr, "Cannot open device (%s)\n", usb_strerror());
		return 2;
	}

	/**************** Setadcchip ****************/
	if (strcmp(eargv[0], "setadcchip")==0) {
		int adc_id, chip_id;
		char *e;

		printf("hmm\n");
	
		/* printf("    setadcchip  adc_id chip\n"); */

		if (n_extra_args<3) {
			fprintf(stderr, "Missing arguments to command\n");
			retval = 1;
			goto cleanAndExit;
		}

		adc_id = strtol(eargv[1], &e, 0);
		if (e==eargv[1]) {
			fprintf(stderr, "Bad adc id\n");
			retval = 1;
			goto cleanAndExit;
		}

		chip_id = strtol(eargv[2], &e, 0);
		if (e==eargv[1]) {
			fprintf(stderr, "Bad chip id\n");
			retval = 1;
			goto cleanAndExit;
		}

		if (g_verbose) 
			printf("Setting adc channel %d to chip_id %d\n", adc_id, chip_id);

		res = usbtenki_command(hdl, USBTENKI_SET_ADC_CHIP, 
								(adc_id & 0xff) | ((chip_id&0xff)<<8), repBuf);
		if (res!=0) {
			fprintf(stderr, "Error setting channel %d to chip_id %d\n",
								adc_id, chip_id);
			retval = 2;
		}

		goto cleanAndExit;
	}


	/***************** Setserial ***************/
	if (strcmp(eargv[0], "setserial")==0) {
		int i, len;

		if (n_extra_args<2) {
			fprintf(stderr, "No serial number specified\n");
			retval = 1;
			goto cleanAndExit;
		}

		len = strlen(eargv[1]);
		if (len != 6) {
			fprintf(stderr, "Serial number must be 6 character long\n");
			retval = 1;
			goto cleanAndExit;
		}

		for (i=0; i<len; i++) {	
			if (g_verbose) 
				printf("Setting serial number character '%c'\n", eargv[1][i]);

			res = usbtenki_command(hdl, USBTENKI_SET_SERIAL, 
									(i&0xff) | (eargv[1][i]<<8), repBuf);
			if (res!=0) {
				fprintf(stderr, "Error writing character '%c'. (%d)\n",
								eargv[1][i], res);
				retval = 2;
				goto cleanAndExit;
			}
		}
		/* index 0xff means store to eeprom. */
		res = usbtenki_command(hdl, USBTENKI_SET_SERIAL, 
								(0xff) | (eargv[1][i]<<8), repBuf);
		if (res != 0) {
			retval = 2;
			goto cleanAndExit;
		}
		
		goto cleanAndExit;
	}
	
	fprintf(stderr, "Unknow command '%s'\n", eargv[0]);

	return 1;

cleanAndExit:
	if (hdl)
		usb_close(hdl);

	return retval;
}
