#include <QDebug>
#include <QSettings>
#include <QLineEdit>
#include "DashSensor.h"
#include "TenkiDevice.h"
#include "SourceAliasEdit.h"
#include "ConfigCheckbox.h"
#include "globals.h"

DashSensor::DashSensor(TenkiDevice *td)
{
	int col =0;
	title = QString::fromAscii(td->getSerial());
	setTitle(title);
	setObjectName("source"); // selector for stylesheet	
	tenki_device = td;

	layout = new QGridLayout();
	setLayout(layout);

	layout->setVerticalSpacing(1);
	layout->setHorizontalSpacing(10);

//	layout->addWidget(new QLabel("<b>Channel</b>"), 0, col++);
	layout->addWidget(new QLabel("<b>Source ID</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Description</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Type</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Current</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Min.</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Max.</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Unit</b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);
	layout->addWidget(new QLabel("<b>Alias</b>"), 0, col++);
	layout->setColumnStretch(col, 0);
	layout->addWidget(new QLabel("<b>Big View</b>"), 0, col++);
	layout->setColumnStretch(col, 0);
	layout->addWidget(new QLabel("<b>Graph</b>"), 0, col++);
	layout->setColumnStretch(col, 0);
	layout->addWidget(new QLabel("<b></b>"), 0, col++);
	layout->setColumnMinimumWidth(col, 4);

	for (int i=0; i<tenki_device->getNumChannels(); i++)
	{
		if (tenki_device->isChannelHidden(i)) {
			continue;
		}

		addChannel(i, i+1);			
	}

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void DashSensor::addChannel(int chn, int row)
{
	QSettings settings;
	USBTenki_channel ch;
	QString a, b, c, d, e, f, g;
	QLabel *value_label, *unit_label;
	QLabel *tmp_label;
	MinMaxResettable *min, *max;
	QPushButton *rst;
	int col=0;

	g_tenkisources->convertToUnits(tenki_device->getChannelData(chn), &ch);

/*	
	a.sprintf("%d", chn);
	layout->addWidget(new QLabel(a), row, col++);*/

	f.sprintf("%s:%02X", tenki_device->getSerial(), chn);
	layout->addWidget(new QLabel(f), row, col++);


	b = QString::fromAscii(chipToString(ch.chip_id));
	tmp_label = new QLabel(b);
//	tmp_label->setWordWrap(true);
//	tmp_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	layout->addWidget(tmp_label, row, col++);
	
	c = QString::fromAscii(chipToShortString(ch.chip_id));
	layout->addWidget(new QLabel(c), row, col++);

	// Current value
	g_tenkisources->formatValue(&d, ch.converted_data);
	value_label = new QLabel(d);
	values.append(value_label);
	layout->addWidget(value_label, row, col++);

	// Minimum value
	min = new MinMaxResettable(true); // Minimum tracking mode
	minimums.append(min);
	layout->addWidget(min, row, col++);

	// Maximum value
	max = new MinMaxResettable(false); // Maximum tracking mode
	maximums.append(max);
	layout->addWidget(max, row, col++);


	e = QString::fromUtf8(unitToString(ch.converted_unit, 0));
	unit_label = new QLabel(e);
	layout->addWidget(unit_label, row, col++);
	units.append(unit_label);

	// alias
	SourceAliasEdit *se = new SourceAliasEdit(f);
	layout->addWidget(se, row, col++);
	connect(se, SIGNAL(sourceAliasChanged(QString,QString)), g_tenkisources, SLOT(updateAlias(QString,QString)));

	// In bigview
	ConfigCheckbox *ccb = new ConfigCheckbox("", "bigviewChecked/" + f);
	layout->addWidget(ccb, row, col++);

	// In graph
	ConfigCheckbox *ccb_gr = new ConfigCheckbox("", "graphChecked/" + f);
	layout->addWidget(ccb_gr, row, col++);

	// Min/Max reset button
	rst = new QPushButton("Reset min./max.");
	rst->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	layout->addWidget(rst, row, col++);
	QObject::connect(rst, SIGNAL(clicked()), min, SLOT(reset()));
	QObject::connect(rst, SIGNAL(clicked()), max, SLOT(reset()));


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
	USBTenki_channel ch, *cdat;
//	qDebug() << "DashSensor::refresh()";

	if (tenki_device->status == TENKI_DEVICE_STATUS_UNABLE_TO_OPEN) {
		title = QString::fromAscii(tenki_device->getSerial());
		title += " [ERROR]";
	} else {
		title = QString::fromAscii(tenki_device->getSerial());
	}
	setTitle(title);

	for (int i=0; i<values.size(); i++) {
		QString d,e;


		cdat = tenki_device->getChannelData(channel_id.at(i));	
		if (!cdat || !cdat->data_valid) {
			values.at(i)->setText("Error");
			continue;
		}
		
		g_tenkisources->convertToUnits(cdat, &ch);		

		g_tenkisources->formatValue(&d, ch.converted_data);
		values.at(i)->setText(d);
		minimums.at(i)->submitValue(ch.converted_data);
		maximums.at(i)->submitValue(ch.converted_data);

		// those two QList are populated in the same
		// order. So it will be the same index i.
		e = QString::fromUtf8(unitToString(ch.converted_unit, 0));
		units.at(i)->setText(e);

	//	qDebug() << d;
	}

}
