#ifndef _usbtenki_priv_h__
#define _usbtenki_priv_h__


#ifdef WINDOWS_VERSION
#include "lusb0_usb.h"
#else
#include <usb.h>
#endif

struct USBTenki_list_ctx {
	struct usb_bus *bus;
	struct usb_device *dev;
};


#endif
