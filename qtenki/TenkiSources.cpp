#include "TenkiSources.h"
#include "usbtenki.h"
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <string.h>

TenkiSources::TenkiSources()
{
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
	struct USBTenki_list_ctx *ctx;
	struct USBTenki_info info;

	ctx = usbtenki_allocListCtx();
	if (!ctx)
		return -1;

	while (usbtenki_listDevices(&info, ctx)) {
		TenkiDevice *td;

		td = new TenkiDevice(info.str_serial);
		device_list.append(td);

		emit newDeviceFound(td);

		addDeviceSources(td);
	}

	usbtenki_freeListCtx(ctx);

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
	QSettings settings;

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

	sd->q_alias = settings.value("sourcesAliases/"+sd->q_name).toString();

	sourceList.append(sd);

	emit changed();

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

void TenkiSources::addSourcesTo(TenkiSourceAddRemove *tsar)
{
	for (int i=0; i<sourceList.size(); i++) {
		tsar->addTenkiSource(sourceList.at(i));
	}
}

void TenkiSources::run()
{
	timer = new QTimer();
	timer->setInterval(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(doCaptures()), Qt::DirectConnection);
	timer->start();
	exec();
}

void TenkiSources::doCaptures()
{
//	qDebug() << "Capture time!";

	for (int i=0; i<device_list.size(); i++) {
//		printf("Updating %s\n", device_list.at(i)->getSerial());
		device_list.at(i)->updateChannelData();
	}

	emit captureCycleCompleted();
}

struct sourceDescription *TenkiSources::getSourceByName(QString source_name)
{
//	qDebug() << "Looking for " + source_name;

	for (int i=0; i<sourceList.size(); i++) {
		struct sourceDescription *sd = sourceList.at(i);
	
//		qDebug() << "Considering " + sd->q_name;
		
		if (sd->q_name == source_name) {
			return sd;
		}
	}

	return NULL;
}

QString TenkiSources::getSourceAliasByName(QString source_name)
{
//	qDebug() << "Looking for " + source_name;

	for (int i=0; i<sourceList.size(); i++) {
		struct sourceDescription *sd = sourceList.at(i);
	
//		qDebug() << "Considering " + sd->q_name;
		
		if (sd->q_name == source_name) {
			return sd->q_alias;
		}
	}

	return "";
}

void TenkiSources::updateAlias(QString source_name, QString alias)
{
	QSettings settings;
	struct sourceDescription *sd;

	sd = getSourceByName(source_name);
	if (sd)
	{
		sd->q_alias = alias;
		settings.setValue("sourcesAliases/"+source_name, alias);
	
		qDebug() << "Updated alias for source '"+source_name+"' for '" + alias + "'";

		emit changed();
	} else {
		qDebug() << "Source not found: "+source_name;
	}
}
