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
	int chn_id;
	
	int chip_id;
	QString chipShortString;
	QString chipString;

	TenkiDevice *td;
	USBTenki_channel *chn_data;
};

class TenkiSources : public QThread
{
	Q_OBJECT

	public:
		TenkiSources();
		~TenkiSources();
		int init();
		int getNumDevices();
		int getNumChannels();
		
		// Add all known TenkiDevices to target
		void syncDevicesTo(TenkiDeviceAddRemove *tdr);

	protected:
		void run();

	public slots:
		void doCaptures();
	
	signals:
		void newDeviceFound(TenkiDevice *td);
		void deviceGone(TenkiDevice *td);

		void captureCycleCompleted();

	private:
		QTimer *timer;
		QList<TenkiDevice*> device_list;
		QList<struct sourceDescription*> sourceList;
		int scanForDevices();
		int addDeviceSources(TenkiDevice *td);
		int addDeviceSource(TenkiDevice *td, int chn_id, struct USBTenki_channel *chndat);
};

#endif // _tenki_sources_h__

