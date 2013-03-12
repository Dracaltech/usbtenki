#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QTabWidget>
#include <QImage>
#include <QMenu>
#include <QScrollArea>

#include "usbtenki.h"
#include "usbtenki_version.h"
#include "TenkiSources.h"
#include "TenkiDashboard.h"
#include "DashSensor.h"
#include "Logger.h"
#include "ConfigPanel.h"
#include "About.h"
#include "globals.h"
#include "BigView.h"
#include "GraphView.h"

MainWindow::MainWindow()
{
	QVBoxLayout *layout = new QVBoxLayout();
	
	tw = new QTabWidget();

	QHBoxLayout *bot_lay = new QHBoxLayout();
	QWidget *bot_btns = new QWidget();
	bot_btns->setLayout(bot_lay);

	QLabel *img_logo = new QLabel();
	img_logo->setPixmap(QPixmap(":rnt_logo200.png"));
	QLabel *txt_logo = new QLabel("raphnet technologies");

	QPushButton *exit_button = new QPushButton(QIcon(":application-exit.png"), QObject::tr("Quit QTenki"));
	
	bot_lay->addWidget(img_logo);
	bot_lay->addWidget(txt_logo);
	bot_lay->addStretch();
	bot_lay->addWidget(exit_button);
	connect(exit_button, SIGNAL(clicked()), this, SLOT(close()));

	QWidget *dash_container;
	QVBoxLayout *dash_container_layout;

	TenkiDashboard *td;
	About *about;

	// If the following changes, configuration will be lost.
	QCoreApplication::setOrganizationName("raphnet technologies");
	QCoreApplication::setOrganizationDomain("raphnet.net");
	QCoreApplication::setApplicationName("Qtenki");
	

	g_tenkisources = new TenkiSources();
	g_tenkisources->init();

	/* prepare tab elements */
	td = new TenkiDashboard();
	dash_container = new QWidget();
	dash_container_layout = new QVBoxLayout();
	dash_container->setLayout(dash_container_layout);
	dash_container_layout->addWidget(td);
	dash_container_layout->addStretch();


	g_tenkisources->syncDevicesTo(td);
	QObject::connect(g_tenkisources, SIGNAL(captureCycleCompleted()), td, SLOT(refreshView()));

	// loggers
	logger = new Logger(g_tenkisources);
	QObject::connect(logger, SIGNAL(loggerStatusChanged(int)), this, SLOT(loggerStatusChanged(int)));

	// big view
	bigView = new BigView();
	QObject::connect(g_tenkisources, SIGNAL(captureCycleCompleted()), bigView, SLOT(refreshView()));

	// graph view
	graphView = new GraphView();

	// messages
	// configuration
	cfgPanel = new ConfigPanel();


	// about
	about = new About();

	QScrollArea *scr_logger = new QScrollArea();
	scr_logger->setWidget(logger);	
	
	QScrollArea *scr_cfgPanel = new QScrollArea();
	scr_cfgPanel->setWidget(cfgPanel);	
	
	QScrollArea *scr_dash = new QScrollArea();
	scr_dash->setWidget(dash_container);

	/* Tabs */
	tw->addTab(scr_dash, QIcon(":sensors.png"), QObject::tr("Sources"));
	tw->addTab(scr_logger, QIcon(":logger.png"), QObject::tr("Logging"));	
	tw->addTab(bigView, QIcon(":view.png"), QObject::tr("Big View"));
	tw->addTab(graphView, QIcon(":view.png"), QObject::tr("Graph View"));
	tw->addTab(scr_cfgPanel, QIcon(":configure.png"), QObject::tr("Configuration"));
	tw->addTab(about, QIcon(":about.png"), QObject::tr("About..."));	

	/* The main window */
	setLayout(layout);

	layout->addWidget(tw);
	layout->addWidget(bot_btns);
	QIcon ico(":qtenki.ico");
	
	setWindowIcon(ico);
	setWindowTitle("QTenki "USBTENKI_VERSION);

	// The tray icon stuff
	trayicon = new QSystemTrayIcon(this);
	trayicon->setIcon(ico);

	connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_show_hide(QSystemTrayIcon::ActivationReason)));
	
	QAction *quit_action = new QAction( "Exit", trayicon);
	connect(quit_action, SIGNAL(triggered()), this, SLOT(close()));

	QAction *hide_action = new QAction( "Show/Hide", trayicon);
	connect(hide_action, SIGNAL(triggered()), this, SLOT(on_show_hide()));

	QMenu *tray_icon_menu = new QMenu;
	tray_icon_menu->addAction(hide_action);
	tray_icon_menu->addAction(quit_action);

	trayicon->setContextMenu(tray_icon_menu);

	readSettings();

	trayicon->show();

}

MainWindow::~MainWindow()
{
}

void MainWindow::readSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
	QSettings settings;

	settings.setValue("geometry", saveGeometry());

	if (logger->confirmMayExit()) {
		// hide tray icon here. Otherwise it lingers until we hover the
		// mouse over it.
		trayicon->hide();
		ev->accept();
		return;
	}

	ev->ignore();
}

void MainWindow::changeEvent(QEvent *ev)
{
	switch (ev->type())
	{
		case QEvent::WindowStateChange:
		{
			if (this->windowState() & Qt::WindowMinimized)
			{
				if (minimize_to_tray) {
					QTimer::singleShot(0, this, SLOT(hide()));
					ev->ignore();
					show();
				}
			}	
		}
		break;

		default:
			break;
	}

	QWidget::changeEvent(ev);
}

void MainWindow::on_show_hide()
{

	if ( isVisible() ) {
		lower();
		if (minimize_to_tray) {
			hide();
		}
	}
	else {
		show();
		raise();
		setFocus();
		showNormal();
	}
}

void MainWindow::on_show_hide(QSystemTrayIcon::ActivationReason reason)
{
	if (reason)
	{
		if (reason != QSystemTrayIcon::DoubleClick)
			return;
	}

	on_show_hide();
}

void MainWindow::loggerStatusChanged(int running)
{
	// Prevent the problem of configured unit changes by disabling
	// the whole configuration dialog.
	cfgPanel->setEnabled(!running);
}
