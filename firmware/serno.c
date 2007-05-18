#include <avr/pgmspace.h>
#include "usbconfig.h"
#include "usbdrv.h"

//int usbDescriptorStringSerialNumber[6] = { SERIAL_NUMBER };
PROGMEM int usbDescriptorStringSerialNumber[] = { 
		USB_STRING_DESCRIPTOR_HEADER(6),
		'B','1','0','0','0','3' 
};

