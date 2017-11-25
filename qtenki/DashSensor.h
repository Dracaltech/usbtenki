#ifndef _dashsensor_h__
#define _dashsensor_h__

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QString>
#include <QList>
#include <QMap>

#include "TenkiDevice.h"
#include "MinMaxResettable.h"

class DashSensor : public QGroupBox
{
	Q_OBJECT

	public:
		DashSensor(TenkiDevice *td);
		~DashSensor();

		TenkiDevice *tenki_device;
		void refresh();

	private:
		void addChannel(int channel, int row);
		QString title;
		QGridLayout *layout;
		QList<QLabel*> descriptions;
		QList<QLabel*> values;
		QList<QLabel*> units;
		QList<MinMaxResettable*> minimums;
		QList<MinMaxResettable*> maximums;
		QList<int> channel_id;
		QList<int> chip_ids;

		void recolorizeThermocouple(void);
		int prev_iec;
};

#endif

