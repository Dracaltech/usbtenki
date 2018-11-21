#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include "TenkiDashboard.h"
#include "DashSensor.h"

TenkiDashboard::TenkiDashboard()
{
	mainLabel = new QLabel();	
	mainLabel->setText("Connected sensor(s):");

	vbox = new QVBoxLayout;

	vbox->addWidget(mainLabel);
	setLayout(vbox);

	nosensorsLabel = new QLabel(tr("No sensor connected, driver not installed or permission denied.<br>Please refer to the manual for instructions."));
	vbox->addWidget(nosensorsLabel);

}

TenkiDashboard::~TenkiDashboard()
{
	delete vbox;
	delete mainLabel;
}

void TenkiDashboard::addDashSensor(DashSensor *ds)
{
	vbox->addWidget(ds);
	vbox->removeWidget(nosensorsLabel);
	nosensorsLabel->setVisible(false);
}

void TenkiDashboard::addTenkiDevice(TenkiDevice *td)
{
	DashSensor *ds;

	ds = new DashSensor(td);
	addDashSensor(ds);
	sensors.append(ds);
}

void TenkiDashboard::removeTenkiDevice(TenkiDevice *td)
{
	// todo 
	//
	// Find the corresponding dash sensor,
	// remove it,
	// free the DashSensor object.
	//
	qDebug() << "TenkiDashboard:: removeTenkiDevice not implemented\n";
	(void)td;
}

void TenkiDashboard::refreshView(void)
{
	for (int i=0; i<sensors.size(); i++) {
		sensors.at(i)->refresh();
	}
}


