/* usbtenkiget: A command-line tool for USBTenki sensors.
 * Copyright (C) 2007-2013  Raphael Assenat <raph@raphnet.net>
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
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include "usbtenki.h"
#include "../common/usbtenki_cmds.h"
#include "../common/usbtenki_version.h"

int g_verbose = 0;

static void printVersion(void)
{
	printf("Usbtenkisetup version %s, Copyright (C) 2007-2013, Raphael Assenat\n\n", USBTENKI_VERSION);
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
	printf("    -f          Operates on the first device found.\n");
	printf("\nValid commands:\n");
	printf("    setadcchip  adc_id chip\n");
	printf("    setserial   serial (8 characters)\n");
	printf("    setref      ref_id (0=AVCC, 1=AREF)\n");
	printf("    set_rtd_cal	value (-127 to +127 (percent / 100))\n");

}

#define MAX_EXTRA_ARGS	8

int main(int argc, char **argv)
{
	int res, i;
	int use_first = 0;
	char *use_serial = NULL;
	int n_extra_args=0;
	char *eargv[MAX_EXTRA_ARGS];
	USBTenki_dev_handle hdl = NULL;
	struct usb_device *cur_dev, *dev=NULL;
	struct USBTenki_list_ctx *listContext;
	struct USBTenki_info info;
	unsigned char repBuf[8];
	int retval = 0;

	while (-1 != (res=getopt(argc, argv, "Vvhfs:")))
	{
		switch (res)
		{
			case 'f':
				use_first = 1;
				break;
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
	if (!use_serial && !use_first) {
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

	usbtenki_init();

	listContext = usbtenki_allocListCtx();

	while ((cur_dev=usbtenki_listDevices(&info, listContext))) {
		if (use_serial) {
			if (strcmp(use_serial, info.str_serial)==0) {
				dev = cur_dev;
				break;
			}
		}
		if (use_first) {
			dev = cur_dev;
			break;
		}
	}

	usbtenki_freeListCtx(listContext);

	if (!dev) {
		fprintf(stderr, "Could not locate device with serial '%s'. Try usbtekiget -l\n",
							use_serial);
		return 1;
	}

	if (n_extra_args<1) {
		fprintf(stderr, "No command specified\n");
		return 1;
	}

	hdl = usbtenki_openDevice(dev);
	if (!hdl) {
		fprintf(stderr, "Cannot open device\n");
		return 2;
	}

	/**************** Setref ****************/
	if (strcmp(eargv[0], "setref")==0) {
		int ref_id;
		char *e;

		/* printf("    setadcchip  adc_id chip\n"); */

		if (n_extra_args<2) {
			fprintf(stderr, "Missing arguments to command\n");
			retval = 1;
			goto cleanAndExit;
		}

		ref_id = strtol(eargv[1], &e, 0);
		if (e==eargv[1]) {
			fprintf(stderr, "Bad ref id\n");
			retval = 1;
			goto cleanAndExit;
		}
		
	if (g_verbose) 
			printf("Setting adc ref to %d\n", ref_id);

		res = usbtenki_command(hdl, USBTENKI_SET_ADC_REF, 
								(ref_id & 0xff), repBuf);
		if (res!=0) {
			fprintf(stderr, "Error setting adc ref to %d\n",
								ref_id);
			retval = 2;
		}

		goto cleanAndExit;
	}

	/**************** Setref ****************/
	if (strcmp(eargv[0], "set_rtd_cal")==0) {
		int ref_id;
		char *e;

		/* printf("    setadcchip  adc_id chip\n"); */

		if (n_extra_args<2) {
			fprintf(stderr, "Missing arguments to command\n");
			retval = 1;
			goto cleanAndExit;
		}

		ref_id = strtol(eargv[1], &e, 0);
		if (e==eargv[1]) {
			fprintf(stderr, "Bad ref id\n");
			retval = 1;
			goto cleanAndExit;
		}
		
	if (g_verbose) 
			printf("Setting rtd calibration to %d\n", ref_id);

		res = usbtenki_command(hdl, USBTENKI_SET_RTD_CORR, 
								(ref_id & 0xff), repBuf);
		if (res!=0) {
			fprintf(stderr, "Error setting adc calibration to %d\n",
								ref_id);
			retval = 2;
		}

		goto cleanAndExit;
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
		usbtenki_closeDevice(hdl);

	return retval;
}
