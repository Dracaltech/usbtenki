#include "TenkiSources.h"
#include "usbtenki.h"
#include <QTimer>
#include <QDebug>

TenkiSources::TenkiSources()
{
	timer = new QTimer();
	timer->setInterval(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(doCaptures()));
}

TenkiSources::~TenkiSources()
{
	// todo : delete all sourceDescriptions
	delete timer;
}

int TenkiSources::init()
{
	usbtenki_init();
	scanForDevices();
	return 0;
}

// TODO : Compare with already existing devices to support Hot plug/unplug.
// For now, please call only once!
int TenkiSources::scanForDevices()
{
	struct USBTenki_list_ctx ctx;
	struct USBTenki_info info;

	usbtenki_initListCtx(&ctx);
	while (usbtenki_listDevices(&info, &ctx)) {
		TenkiDevice *td;

		td = new TenkiDevice(info.str_serial);
		device_list.append(td);

		emit newDeviceFound(td);

		addDeviceSources(td);
	}

	return 0;
}

int TenkiSources::addDeviceSources(TenkiDevice *td)
{
	int chn = td->getNumChannels();

	for (int i=0; i<chn; i++) {
		if (!td->isChannelHidden(i)) {
			addDeviceSource(td, i, &td->channel_data[i]);
		}
	}

	return 0;
}

int TenkiSources::addDeviceSource(TenkiDevice *td, int chn_id, struct USBTenki_channel *chndat)
{
	struct sourceDescription *sd;
	const char *serial = td->getSerial();

	if (strlen(serial) > 8) {
		return -1;
	}

	sd = new struct sourceDescription;
	if (!sd)
		return -1;

	sprintf(sd->name, "%s:%02X", serial, chn_id);
	sd->q_name = QString::fromAscii(sd->name);
	printf("TenkiSources: Registering source '%s'\n", sd->name);

	sd->td = td;
	sd->chn_id = chn_id;

	sd->chipShortString = QString::fromAscii(chipToShortString(chndat->chip_id));
	sd->chipString = QString::fromAscii(chipToString(chndat->chip_id));
	sd->chn_data = chndat;

	sourceList.append(sd);

	return 0;
}

// TODO : Add / remove
void TenkiSources::syncDevicesTo(TenkiDeviceAddRemove *tdr)
{
	for (int i=0; i<device_list.size(); i++)
	{
		tdr->addTenkiDevice(device_list.at(i));
	}
}

void TenkiSources::run()
{
	timer->start();
}

void TenkiSources::doCaptures()
{
	qDebug() << "Capture time!";

	for (int i=0; i<device_list.size(); i++) {
		printf("Updating %s\n", device_list.at(i)->getSerial());
		device_list.at(i)->updateChannelData();
	}

	emit captureCycleCompleted();
}

