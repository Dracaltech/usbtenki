#ifndef _tenki_sources_h__
#define _tenki_sources_h__


#include <QThread>
#include <QTimer>
#include <QList>
#include "TenkiDevice.h"
#include "usbtenki.h"

struct sourceDescription {
	char name[32]; // XXXXXX:XX form.
	QString q_name;
	QString q_alias;
	int chn_id;
	
	int chip_id;
	QString chipShortString;
	QString chipString;

	TenkiDevice *td;
	USBTenki_channel *chn_data;
};

class TenkiSourceAddRemove
{
	public:
		virtual void addTenkiSource(struct sourceDescription *sd) = 0;
		virtual void removeTenkiSource(struct sourceDescription *sd) = 0;
};


class TenkiSources : public QThread
{
	Q_OBJECT

	public:
		TenkiSources();
		~TenkiSources();
		int init();
		int getNumDevices();
		
		// Add all known TenkiDevices to target
		void syncDevicesTo(TenkiDeviceAddRemove *tdr);
		void addSourcesTo(TenkiSourceAddRemove *tsar);

		struct sourceDescription *getSourceByName(QString src);
		QString getSourceAliasByName(QString src);

		void setTemperatureUnit(int temperature_unit);
		void setPressureUnit(int pressure_unit);
		void setFrequencyUnit(int frequency_unit);
		void setInterval_ms(int interval);

		void convertToUnits(const struct USBTenki_channel *chn, struct USBTenki_channel *dst);
	protected:
		void run();

	public slots:
		void doCaptures();
		void updateAlias(QString source_name, QString alias);
	
	signals:
		void newDeviceFound(TenkiDevice *td);
		void deviceGone(TenkiDevice *td);

		void captureCycleCompleted();
		void changed();

	private:
		QTimer *timer;
		QList<TenkiDevice*> device_list;
		QList<struct sourceDescription*> sourceList;
		int scanForDevices();
		int addDeviceSources(TenkiDevice *td);
		int addDeviceSource(TenkiDevice *td, int chn_id, struct USBTenki_channel *chndat);
		int pressure_unit;
		int temperature_unit;
		int frequency_unit;
};

#endif // _tenki_sources_h__

