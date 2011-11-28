#include "TenkiDevice.h"
#include "../common/usbtenki_cmds.h"
#include "TenkiGlue.h"

TenkiDevice::TenkiDevice(const char *serial)
{
	tenki_hdl = usbtenki_openBySerial(serial, &tenki_info);
	if (tenki_hdl != NULL) {
		status = TENKI_DEVICE_STATUS_OK;
		initChannels();
//		updateChannelData();
	} else {
		status = TENKI_DEVICE_STATUS_UNABLE_TO_OPEN;
	}

}

TenkiDevice::~TenkiDevice()
{
	usb_close(tenki_hdl);
	tenki_hdl = 0;
}

void TenkiDevice::initChannels()
{
	num_channels = usbtenki_getNumChannels(tenki_hdl);
	if (num_channels > MAX_CHANNELS)
		num_channels = MAX_CHANNELS;

	usbtenki_listChannels(tenki_hdl, channel_data, MAX_CHANNELS);

	for (int i=0; i<num_channels; i++) {
		if (!isChannelHidden(i)) {
			tenkiglue_registerSource(this, tenki_info.str_serial, i, &channel_data[i]);
		}
	}
}

void TenkiDevice::updateChannelData()
{
	int i;
	for (i=0; i<num_channels; i++) {
		if (channel_data[i].chip_id == USBTENKI_CHIP_NONE)
			continue;

		usbtenki_readChannel(tenki_hdl, &channel_data[i]);
	}
}

int TenkiDevice::isChannelHidden(int id)
{
	return (channel_data[id].chip_id == USBTENKI_CHIP_NONE);
}

const char *TenkiDevice::getSerial(void)
{
	return tenki_info.str_serial;
}

int TenkiDevice::getNumChannels(void)
{
	return num_channels;
}

USBTenki_channel *TenkiDevice::getChannelData(int id)
{
	return &channel_data[id];
}
