#ifndef _pressure_preference_h__
#define _pressure_preference_h__

#include <QtGui>

class PressurePreference : public QComboBox
{
	Q_OBJECT

	public:
		PressurePreference();
		int tenkiUnit();

	private slots:
		void idx_changed(int idx);

	private:
		QString config_key;
};



#endif // _temperature_preference_h__

