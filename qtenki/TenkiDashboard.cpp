#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include "TenkiDashboard.h"
#include "DashSensor.h"

TenkiDashboard::TenkiDashboard(TenkiSources *src)
{
	mainLabel = new QLabel();	
	mainLabel->setText("Connected sensor(s):");

	vbox = new QVBoxLayout;

	vbox->addWidget(mainLabel);
	setLayout(vbox);

	tsrc = src;
}

TenkiDashboard::~TenkiDashboard()
{
	delete vbox;
	delete mainLabel;
}

void TenkiDashboard::addDashSensor(DashSensor *ds)
{
	vbox->addWidget(ds);
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
}

void TenkiDashboard::refreshView(void)
{
	for (int i=0; i<sensors.size(); i++) {
		sensors.at(i)->refresh();
	}
}


