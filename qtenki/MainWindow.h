#ifndef _MainWindow_h__
#define _MainWindow_h__

#include <QWidget>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QTabWidget>

#include "Logger.h"
#include "ConfigPanel.h"
#include "BigView.h"

class MainWindow : public QWidget
{
	Q_OBJECT

	public:
		MainWindow();
		~MainWindow();

	protected:
		virtual void closeEvent(QCloseEvent *ev);
		virtual void changeEvent(QEvent *e);

	private slots:
		void on_show_hide(QSystemTrayIcon::ActivationReason reason);
		void on_show_hide();
		void loggerStatusChanged(int running);

	private:
		Logger *logger;
		ConfigPanel *cfgPanel;
		BigView *bigView;
		QSystemTrayIcon *trayicon;
		QTabWidget *tw;
};

#endif // _MainWindow_h__

