#include <QDebug>
#include "DashSensor.h"
#include "TenkiDevice.h"

DashSensor::DashSensor(TenkiDevice *td)
{
	title = QString::fromAscii(td->getSerial());
	setTitle(title);
	
	tenki_device = td;

	layout = new QGridLayout();
	setLayout(layout);

	layout->setSpacing(10);

	layout->addWidget(new QLabel("<b>Channel</b>"), 0, 0);
	layout->addWidget(new QLabel("<b>Description</b>"), 0, 1);
	layout->addWidget(new QLabel("<b>Type</b>"), 0, 2);
	layout->addWidget(new QLabel("<b>Value</b>"), 0, 3);
	layout->addWidget(new QLabel("<b>Unit</b>"), 0, 4);
	layout->addWidget(new QLabel("<b>Source ID</b>"), 0, 5);

	for (int i=0; i<tenki_device->getNumChannels(); i++)
	{
		if (tenki_device->isChannelHidden(i)) {
			continue;
		}

		addChannel(i, i+1);			
	}
}

void DashSensor::addChannel(int chn, int row)
{
	USBTenki_channel *ch;
	QString a, b, c, d, e, f;
	QLabel *value_label, *unit_label;

	ch = tenki_device->getChannelData(chn);
	
	a.sprintf("%d", chn);
	layout->addWidget(new QLabel(a), row, 0);

	b = QString::fromAscii(chipToString(ch->chip_id));
	layout->addWidget(new QLabel(b), row, 1);
	
	c = QString::fromAscii(chipToShortString(ch->chip_id));
	layout->addWidget(new QLabel(c), row, 2);

	d.sprintf("%.3f", ch->converted_data);	
	value_label = new QLabel(d);
	values.append(value_label);
	layout->addWidget(value_label, row, 3);

	e = QString::fromUtf8(unitToString(ch->converted_unit, 0));
	unit_label = new QLabel(e);
	layout->addWidget(unit_label, row, 4);
	units.append(unit_label);

	f.sprintf("%s:%02x", tenki_device->getSerial(), chn);
	layout->addWidget(new QLabel(f), row, 5);

	channel_id.append(chn);
}

DashSensor::~DashSensor()
{
	QLayoutItem *child;
	while ((child = layout->takeAt(0)) != 0) {
		delete child;
	}
	delete layout;
}


void DashSensor::refresh()
{
	USBTenki_channel *ch;
//	qDebug() << "DashSensor::refresh()";

	for (int i=0; i<values.size(); i++) {
		QString d,e;

		ch = tenki_device->getChannelData(channel_id.at(i));	
		if (!ch->data_valid) {
			values.at(i)->setText("Error");
			continue;
		}
		d.sprintf("%.3f", ch->converted_data);	
		values.at(i)->setText(d);

		// those two QList are populated in the same
		// order. So it will be the same index i.
		e = QString::fromUtf8(unitToString(ch->converted_unit, 0));
		units.at(i)->setText(e);

	//	qDebug() << d;
	}

}
