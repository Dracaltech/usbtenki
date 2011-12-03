#include <QtGui>

#include "usbtenki.h"
#include "TenkiSources.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
#include "Logger.h"

struct USBTenki_list_ctx list_context;
struct USBTenki_info tenki_info;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QVBoxLayout *layout = new QVBoxLayout();
	QTabWidget *tw = new QTabWidget();
	QWidget *window = new QWidget();

	TenkiDashboard *td;
	Logger *logger;

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

	// loggers
	logger = new Logger(tenkisources);

	// messages
	// configuration
	
	/* Tabs */
	tw->addTab(td, QObject::tr("Sources"));
	tw->addTab(logger, QObject::tr("Logging"));	

	/* The main window */
	window->setLayout(layout);
	layout->addWidget(tw);


	window->show();

	
	return app.exec();
}
