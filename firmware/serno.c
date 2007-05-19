#include <string.h>
#include <avr/pgmspace.h>
#include "usbconfig.h"
#include "usbdrv.h"
#include "eeprom.h"
#include "serno.h"

//int usbDescriptorStringSerialNumber[6] = { SERIAL_NUMBER };
int usbDescriptorStringSerialNumber[] = { 
		USB_STRING_DESCRIPTOR_HEADER(6),
		'A','B','C','D','E','F' 
};

void serno_setChar(int idx, unsigned char v)
{
	if (idx<0)
		return;

	if (idx>=sizeof(g_eeprom_data.serial))
		return;

	g_eeprom_data.serial[idx] = v;
}

void serno_store(void)
{
	eeprom_commit();
	int i;
	for (i=0; i<6; i++) {
		usbDescriptorStringSerialNumber[i+1] =
							g_eeprom_data.serial[i];
	}
}

void serno_init()
{
	int i;
	for (i=0; i<6; i++) {
		usbDescriptorStringSerialNumber[i+1] =
							g_eeprom_data.serial[i];
	}
}


