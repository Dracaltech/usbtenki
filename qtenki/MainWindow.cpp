#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QTabWidget>

#include "usbtenki.h"
#include "TenkiSources.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
#include "Logger.h"
#include "ConfigPanel.h"
#include "About.h"

MainWindow::MainWindow()
{
	QVBoxLayout *layout = new QVBoxLayout();
	QTabWidget *tw = new QTabWidget();

	QHBoxLayout *bot_lay = new QHBoxLayout();
	QWidget *bot_btns = new QWidget();
	bot_btns->setLayout(bot_lay);
	bot_lay->addStretch();
	QPushButton *exit_button = new QPushButton(QIcon(":application-exit.png"), QObject::tr("Quit QTenki"));
	bot_lay->addWidget(exit_button);
	connect(exit_button, SIGNAL(clicked()), this, SLOT(close()));

	QWidget *dash_container;
	QVBoxLayout *dash_container_layout;

	TenkiDashboard *td;
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
	cfgPanel = new ConfigPanel();

	// about
	about = new About();
	
	/* Tabs */
	tw->addTab(dash_container, QIcon(":sensors.png"), QObject::tr("Sources"));
	tw->addTab(logger, QIcon(":logger.png"), QObject::tr("Logging"));	
	tw->addTab(cfgPanel, QIcon(":configure.png"), QObject::tr("Configuration"));
	tw->addTab(about, QIcon(":about.png"), QObject::tr("About..."));	

	/* The main window */
	setLayout(layout);

	layout->addWidget(tw);
	layout->addWidget(bot_btns);
	QIcon ico(":qtenki.ico");
	setWindowIcon(ico);
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
	if (logger->confirmMayExit()) {
		ev->accept();
		return;
	}

	ev->ignore();
}
