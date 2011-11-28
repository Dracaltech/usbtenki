#ifndef _tenki_device_h__
#define _tenki_device_h__

#include "usbtenki.h"

#define MAX_CHANNELS	32

#define TENKI_DEVICE_STATUS_OK					0
#define TENKI_DEVICE_STATUS_UNABLE_TO_OPEN		1


class TenkiDevice
{
	public:
		 TenkiDevice(const char *serial);
		~TenkiDevice();
		
		const char *getSerial(void);		
		int getNumChannels(void);
		USBTenki_channel *getChannelData(int id);
		int isChannelHidden(int id);
	
		int status;
		
	private:
		void initChannels();
		void updateChannelData();
		usb_dev_handle *tenki_hdl;
		struct USBTenki_info tenki_info;
		struct USBTenki_channel channel_data[MAX_CHANNELS];
		int num_channels;
};

#endif // _tenki_device_h__

