#ifndef _usbtemp_h__
#define _usbtemp_h__

#include <usb.h>

#define OUR_VENDOR_ID 	0x1781
#define OUR_PRODUCT_ID 	0x0a98
#define ID_STRING		"USB_Temp"

#define TEMPFMT_CELCIUS		0x00
#define TEMPFMT_FAHRENHEIT	0x01
#define TEMPFMT_KELVIN		0x02

struct USBtemp_info {
	char str_prodname[256];
	char str_serial[256];
	int major, minor;
};

struct USBtemp_list_ctx {
	struct usb_bus *bus;
	struct usb_device *dev;
};

void usbtemp_initListCtx(struct USBtemp_list_ctx *ctx);
struct usb_device *usbtemp_listDevices(struct USBtemp_info *info, 
										struct USBtemp_list_ctx *ctx);

int usbtemp_getRaw(usb_dev_handle *hdl, int id, unsigned char *dst);
int usbtemp_getChipID(usb_dev_handle *hdl, int id);
int usbtemp_getNumChannels(usb_dev_handle *hdl);

int usbtemp_printTemperature(usb_dev_handle *hdl, int id, int fmt, int n_avg);

const char *chipToString(int id);

#endif // _rgbleds_h__

