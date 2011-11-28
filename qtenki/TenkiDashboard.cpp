#include <QtGui>

#include "TenkiDashboard.h"
#include "DashSensor.h"

TenkiDashboard::TenkiDashboard()
{
	mainLabel = new QLabel();	
	mainLabel->setText("Connected sensor(s):");

	vbox = new QVBoxLayout;

	vbox->addWidget(mainLabel);
	setLayout(vbox);
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
