#ifndef _usbtenki_priv_h__
#define _usbtenki_priv_h__

#include <usb.h>

struct USBTenki_list_ctx {
	struct usb_bus *bus;
	struct usb_device *dev;
};


#endif
