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

		int updateChannelData();
		struct USBTenki_channel channel_data[MAX_CHANNELS];

	private:
		void initChannels();
		USBTenki_dev_handle tenki_hdl;
		struct USBTenki_info tenki_info;
		int num_channels;
		char *serno;
};

class TenkiDeviceAddRemove
{
	public:
		virtual void addTenkiDevice(TenkiDevice *td) = 0;
		virtual void removeTenkiDevice(TenkiDevice *td) = 0;
};

#endif // _tenki_device_h__

