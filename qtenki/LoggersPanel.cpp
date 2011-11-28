#include "LoggersPanel.h"
#include "TenkiDevice.h"
#include "CreateLogger.h"
#include <QMessageBox>
#include <QPushButton>

LoggersPanel::LoggersPanel()
{
	

	listLayout = new QGridLayout();
	listArea.setLayout(listLayout);

	listLayout->setSpacing(10);

	listLayout->addWidget(new QLabel("<b>Name</b>"), 0, 0);
	listLayout->addWidget(new QLabel("<b>Running</b>"), 0, 1);
	listLayout->addWidget(new QLabel("<b>Management</b>"), 0, 2);

	mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	mainLayout->addWidget(&listArea, 0, Qt::AlignTop);

	QPushButton *createLogger = new QPushButton("Create new logger...");
	mainLayout->addWidget(createLogger);

	QObject::connect(createLogger, SIGNAL(clicked()), this, SLOT(openCreator()));

}

void LoggersPanel::addLogger()
{
	/*
	USBTenki_channel *ch;
	QString a, b, c, d, e, f;
	QLabel *value_label;

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

	e = QString::fromAscii(unitToString(ch->converted_unit, 0));
	layout->addWidget(new QLabel(e), row, 4);

	f.sprintf("%s:%02x", tenki_device->getSerial(), chn);
	layout->addWidget(new QLabel(f), row, 5);*/
}

LoggersPanel::~LoggersPanel()
{
#if 0
	QLayoutItem *child;
	while ((child = listLayout->takeAt(0)) != 0) {
		delete child;
	}
	while ((child = mainLayout->takeAt(0)) != 0) {
		delete child;
	}
	// listLayout is in mainLayout.
	delete mainLayout;
#endif
}

void LoggersPanel::openCreator(void)
{
	CreateLogger cd;

	cd.exec();
}

