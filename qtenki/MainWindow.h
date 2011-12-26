#ifndef _MainWindow_h__
#define _MainWindow_h__

#include <QWidget>
#include <QCloseEvent>

#include "Logger.h"

class MainWindow : public QWidget
{
	Q_OBJECT

	public:
		MainWindow();
		~MainWindow();

	protected:
		virtual void closeEvent(QCloseEvent *ev);

	private:
		Logger *logger;
};

#endif // _MainWindow_h__

