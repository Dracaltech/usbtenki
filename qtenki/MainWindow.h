#ifndef _MainWindow_h__
#define _MainWindow_h__

#include <QWidget>
#include <QCloseEvent>
#include <QSystemTrayIcon>

#include "Logger.h"
#include "ConfigPanel.h"

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

	private:
		Logger *logger;
		ConfigPanel *cfgPanel;
		QSystemTrayIcon *trayicon;
};

#endif // _MainWindow_h__

