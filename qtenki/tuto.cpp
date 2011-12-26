#include <QtGui>

#include "usbtenki.h"
#include "TenkiSources.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
#include "Logger.h"
#include "About.h"

struct USBTenki_list_ctx list_context;
struct USBTenki_info tenki_info;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QVBoxLayout *layout = new QVBoxLayout();
	QTabWidget *tw = new QTabWidget();
	QWidget *window = new QWidget();

	QWidget *dash_container;
	QVBoxLayout *dash_container_layout;

	TenkiDashboard *td;
	Logger *logger;
	About *about;

	QCoreApplication::setOrganizationName("raphnet technologies");
	QCoreApplication::setOrganizationDomain("raphnet.net");
	QCoreApplication::setApplicationName("QTenki");

	TenkiSources *tenkisources = new TenkiSources();
	tenkisources->init();
	tenkisources->start();

	/* prepare tab elements */
	td = new TenkiDashboard(tenkisources);
	dash_container = new QWidget();
	dash_container_layout = new QVBoxLayout();
	dash_container->setLayout(dash_container_layout);
	dash_container_layout->addWidget(td);
	dash_container_layout->addStretch();


	tenkisources->syncDevicesTo(td);
	QObject::connect(tenkisources, SIGNAL(captureCycleCompleted()), td, SLOT(refreshView()));

	// loggers
	logger = new Logger(tenkisources);

	// messages
	// configuration

	// about
	about = new About();
	
	/* Tabs */
	tw->addTab(dash_container, QObject::tr("Sources"));
	tw->addTab(logger, QObject::tr("Logging"));	
	tw->addTab(about, QObject::tr("About..."));	

	/* The main window */
	window->setLayout(layout);
	layout->addWidget(tw);


	window->show();

	
	return app.exec();
}
