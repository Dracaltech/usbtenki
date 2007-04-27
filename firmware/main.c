#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "oddebug.h"

#include "mcp9800.h"
#include "i2c.h"
#include "usbtemp_cmds.h"

int usbCfgSerialNumberStringDescriptor[] PROGMEM = {
	USB_STRING_DESCRIPTOR_HEADER(USB_CFG_SERIAL_NUMBER_LENGTH),
	'1', '0', '0', '0'
};
#define MCP9800_ADDR	(MCP9800_ADDR_BASE + 7)

static char g_auto_mode = 1;

uchar   usbFunctionSetup(uchar data[8])
{
	static uchar    replyBuf[3];
	int replen=0;

	g_auto_mode = 0;
	
    usbMsgPtr = replyBuf;

	switch (data[1])
	{
		case USBTEMP_ECHO:
        	replyBuf[0] = data[2];
	        replyBuf[1] = data[3];
			replen = 2;
			break;
		case USBTEMP_GET_RAW:
			replyBuf[0] = 
				mcp9800_readRegister(MCP9800_ADDR, MCP9800_REG_TEMP,
													&replyBuf[1], 2);
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

	i2c_init();
	if (mcp9800_configure(MCP9800_ADDR, MCP9800_CFG_12BITS))
		while(1) { /* watchdog will reset me! */ }
	

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

