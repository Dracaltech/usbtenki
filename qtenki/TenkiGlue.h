#ifndef _tenkiglue_h__
#define _tenkiglue_h__

#include <QtGui>
#include "TenkiDashboard.h"
#include "TenkiDevice.h"
#include "DataSourceCheckBox.h"

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

int tenkiglue_shutdown();
int tenkiglue_init();
int tenkiglue_syncDashboard(TenkiDashboard *td);
int tenkiglue_registerSource(TenkiDevice *td, const char *serial, int chn_id, USBTenki_channel *chn);
int tenkiglue_populateSourceCheckboxes(QLayout *l, QList<DataSourceCheckBox*>*sources);


#endif 

