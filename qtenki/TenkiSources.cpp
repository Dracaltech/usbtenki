#include "TenkiSources.h"
#include "usbtenki.h"
#include <QTimer>
#include <QDebug>
#include <QSettings>
#include <string.h>
#include "usbtenki_units.h"

TenkiSources::TenkiSources()
{
	QSettings settings;

	pressure_unit = TENKI_UNIT_KPA;
	temperature_unit = TENKI_UNIT_CELCIUS;
	frequency_unit = TENKI_UNIT_HZ;
}

TenkiSources::~TenkiSources()
{
	// todo : delete all sourceDescriptions
	delete timer;
}

void TenkiSources::setTemperatureUnit(int tenki_temp_unit)
{
	this->temperature_unit = tenki_temp_unit;
}

void TenkiSources::setPressureUnit(int pressure_unit)
{
	this->pressure_unit = pressure_unit;
}

void TenkiSources::setFrequencyUnit(int frequency_unit)
{
	this->frequency_unit = frequency_unit;
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

void TenkiSources::convertToUnits(const struct USBTenki_channel *chn, struct USBTenki_channel *dst)
{
	struct USBTenki_channel tmp;

	memcpy(&tmp, chn, sizeof(tmp));
	usbtenki_convertUnits(&tmp, temperature_unit, pressure_unit, frequency_unit);
	memcpy(dst, &tmp, sizeof(tmp));
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
