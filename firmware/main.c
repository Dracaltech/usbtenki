/*   USBTenki - Interfacing sensors to USB 
 *   Copyright (C) 2007-2011  Raphaël Assénat <raph@raphnet.net>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "usbdrv.h"
#include "oddebug.h"
#include "usbconfig.h"
#include "interface.h"

#include "i2c.h"
#include "eeprom.h"
#include "serno.h"
#include "adc.h"
#include "usbtenki_cmds.h"

static char g_auto_mode = 1;


void usbtenki_delay_ms(int ms)
{
	int i;

	for (i=0; i<(ms/10); i++) {
        wdt_reset();
        usbPoll();
		_delay_ms(10);
	}
	usbPoll();
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

uchar   usbFunctionSetup(uchar data[8])
{
	static uchar    replyBuf[8];
	int replen=0, res;
	int total_channels;
	int sensors_channels;
	int num_adc_channels=EEPROM_ADC_CHIPS_SIZE;

	g_auto_mode = 0;
	sensors_channels = sensors_getNumChannels();
	total_channels = sensors_channels + num_adc_channels;
	
    usbMsgPtr = replyBuf;

	switch (data[1])
	{
		case USBTENKI_GET_RAW:
			if (data[2] >= total_channels) 
				break;

			replyBuf[0] = USBTENKI_GET_RAW;

			if (data[2] >= sensors_channels)
			{
				unsigned short val;
//				ADCSRA |= (1<<ADSC); /* start conversion */
//				while (!(ADCSRA & (1<<ADIF))) 
//					{ /* do nothing... */ };
//				replyBuf[2] = ADCL;
//				replyBuf[1] = ADCH;
				// Take 5 samples at 10ms intervals and
				// average them.
				val = adc_sample(data[2] - sensors_channels,
								5,
								10);
				replyBuf[1] = val >> 8;
				replyBuf[2] = val & 0xff;
				res = 2;
			}
			else
			{
				res = sensors_getRaw(data[2], &replyBuf[1]);

				if (res<0) {
					replyBuf[0] = USBTENKI_ERROR;
					replen = 1;
					break;
				}
			}

			replyBuf[res+1] = xor_buf(replyBuf, res+1);	
			replen = res + 2;
			break;

		case USBTENKI_GET_CHIP_ID:
			if (data[2] >= total_channels) 
				break;

			replyBuf[0] = USBTENKI_GET_CHIP_ID;
			if (data[2] >= sensors_channels) {
				replyBuf[1] = g_eeprom_data.adc_chips[(data[2]-sensors_channels)];
			} else {
				replyBuf[1] = sensors_getChipID(data[2]);
			}
			replyBuf[2] = xor_buf(replyBuf, 2);
			replen = 3;
			break;

		case USBTENKI_GET_NUM_CHANNELS:
			replyBuf[0] = USBTENKI_GET_NUM_CHANNELS;
			replyBuf[1] = total_channels;
			replyBuf[2] = xor_buf(replyBuf, 2);
			replen = 3;
			break;

		case USBTENKI_SET_SERIAL:
			if (data[2] == 0xff) {
				serno_store();
			} else {
				serno_setChar(data[2], data[3]);
			}
			replyBuf[0] = USBTENKI_SET_SERIAL;
			replyBuf[1] = xor_buf(replyBuf, 1);
			replen = 2;
			break;

		case USBTENKI_SET_ADC_CHIP:
			if (data[2] >= num_adc_channels)
				break;

			g_eeprom_data.adc_chips[data[2]] = data[3];
			eeprom_commit();

			replyBuf[0] = USBTENKI_SET_ADC_CHIP;
			replyBuf[1] = xor_buf(replyBuf, 1);
			replen = 2;
			break;

		case USBTENKI_SET_ADC_REF:
			g_eeprom_data.use_aref = data[2];
			eeprom_commit();

			replyBuf[0] = USBTENKI_SET_ADC_REF;
			replyBuf[1] = xor_buf(replyBuf, 1);
			replen = 2;
			break;

    }
	
	return replen;
}

int main(void)
{
	uchar   i, j;

    wdt_enable(WDTO_1S);
    odDebugInit();

	PORTB = 0xff;
	DDRB = 0xff;
	
	// all input by default
	PORTC= 0xff;
	DDRC = 0x00;

	adc_init();
	eeprom_init();
	serno_init();

	

	/* 1101 1000 bin: activate pull-ups except on USB lines
	 *
	 * USB signals are on bit 0 and 2.
	 *
	 * Bit 1 is connected with bit 0 (rev.C pcb error), so the pullup
	 * is not enabled.
	 */
    PORTD = 0xf8;
    DDRD = 0x01|0x04;
    j = 0;
    while(--j){		/* USB Reset by device only required on Watchdog Reset */
        i = 0;
        while(--i);	/* delay >10ms for USB reset */
    }
	DDRD &= ~0x05;	/* 0000 0010 bin: remove USB reset condition */

    usbInit();
    sei();

	if (sensors_init()) {
		while(1) { } /* watchdog will reset me! */
	}

    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();
    }


    return 0;
}

