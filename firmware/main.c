#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "oddebug.h"
#include "usbconfig.h"
#include "interface.h"

#include "i2c.h"
#include "usbtenki_cmds.h"

static char g_auto_mode = 1;

static int num_adc_channels=1;

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
				ADCSRA |= (1<<ADSC); /* start conversion */
				while (!(ADCSRA & (1<<ADIF))) 
					{ /* do nothing... */ };
				replyBuf[2] = ADCL;
				replyBuf[1] = ADCH;
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
				replyBuf[1] = USBTENKI_MCU_ADC0 + (data[2]-sensors_channels);
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
    }
	
	return replen;
}

int main(void)
{
	uchar   i, j;

    wdt_enable(WDTO_1S);
    odDebugInit();

	
	// all input by default
	PORTC= 0xff;
	DDRC = 0x00;

	/* Use AVCC (usb 5 volts) and select ADC0. */
	ADMUX = (1<<REFS0);
	/* Enable ADC and setup prescaler to /128 (gives 93khz) */
	ADCSRA = (1<<ADEN) | 
		(1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	

	i2c_init();

	if (sensors_init()) {
		while(1) { } /* watchdog will reset me! */
	}

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
    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();
    
    }


    return 0;
}

