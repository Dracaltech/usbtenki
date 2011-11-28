#ifndef _tenki_dashboard_h__
#define _tenki_dashboard_h__

#include <QWidget>
#include "DashSensor.h"

class TenkiDashboard : public QWidget
{
	Q_OBJECT

	public:
		TenkiDashboard();
		~TenkiDashboard();
		void addDashSensor(DashSensor *ds);
	
	private:
		QLabel *mainLabel;
		QVBoxLayout *vbox;
};

#endif

