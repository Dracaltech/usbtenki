#include <QtGui>

#include "usbtenki.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
#include "TenkiGlue.h"
#include "LoggersPanel.h"

struct USBTenki_list_ctx list_context;
struct USBTenki_info tenki_info;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QVBoxLayout layout;
	QTabWidget tw;
	TenkiDashboard td;
	QWidget window;
	LoggersPanel loggers;

	QCoreApplication::setOrganizationName("raphnet technologies");
	QCoreApplication::setOrganizationDomain("raphnet.net");
	QCoreApplication::setApplicationName("QTenki");


	/* prepare tab elements */
	// loggers
	// messages
	// configuration

	/* Tabs */
	tw.addTab(&td, QObject::tr("Dashboard"));
	tw.addTab(&loggers, QObject::tr("Loggers"));	

	/* The main window */

	tenkiglue_init();
	tenkiglue_syncDashboard(&td);


	window.setLayout(&layout);
	layout.addWidget(&tw);

	window.show();

	return app.exec();
}
