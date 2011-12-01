#include <QtGui>

#include "usbtenki.h"
#include "TenkiSources.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
//#include "TenkiGlue.h"
#include "LoggersPanel.h"

struct USBTenki_list_ctx list_context;
struct USBTenki_info tenki_info;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QVBoxLayout *layout = new QVBoxLayout();
	QTabWidget *tw = new QTabWidget();
	QWidget *window = new QWidget();

	TenkiDashboard *td;
	LoggersPanel *loggers;

	QCoreApplication::setOrganizationName("raphnet technologies");
	QCoreApplication::setOrganizationDomain("raphnet.net");
	QCoreApplication::setApplicationName("QTenki");

	TenkiSources *tenkisources = new TenkiSources();
	tenkisources->init();
	tenkisources->start();

	/* prepare tab elements */
	td = new TenkiDashboard(tenkisources);

	tenkisources->syncDevicesTo(td);

	QObject::connect(tenkisources, SIGNAL(captureCycleCompleted()), td, SLOT(refreshView()));

/*
	// loggers
	loggers = new LoggersPanel();

	// messages
	// configuration
*/
	/* Tabs */
	tw->addTab(td, QObject::tr("Dashboard"));
//	tw->addTab(loggers, QObject::tr("Loggers"));	

	/* The main window */
	window->setLayout(layout);
	layout->addWidget(tw);

	window->show();

	
	return app.exec();
}
