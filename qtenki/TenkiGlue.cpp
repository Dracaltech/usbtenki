#include "usbtenki.h"
#include <stdio.h>
#include <QList>
#include <QCheckBox>
#include <QLayout>
#include "TenkiDashboard.h"
#include "TenkiDevice.h"
#include "DashSensor.h"
#include "TenkiGlue.h"

static QList<TenkiDevice*> device_list;
static QList<DashSensor*> dash_list;


static QList<struct sourceDescription*> sourceList;

int tenkiglue_init()
{
	struct USBTenki_list_ctx ctx;
	struct USBTenki_info info;

	// prepare library
	usbtenki_init();

	
	// build TenkiDevice objects
	usbtenki_initListCtx(&ctx);	
	while (usbtenki_listDevices(&info, &ctx)) {
		TenkiDevice *td;

		td = new TenkiDevice(info.str_serial);
		device_list.append(td);
	}

	return 0;
}

int tenkiglue_populateSourceCheckboxes(QLayout *l, QList<DataSourceCheckBox*>*sources)
{
	for (int i=0; i<sourceList.size(); i++) {
		struct sourceDescription *sd = sourceList.at(i);

		DataSourceCheckBox *cb = new DataSourceCheckBox(sd->q_name + "  --   " + sd->chipShortString, sd->q_name);
		l->addWidget(cb);
		if (sources)
			sources->append(cb);
	}
	return sourceList.size();
}

int tenkiglue_registerSource(TenkiDevice *td, const char *serial, int chn_id, USBTenki_channel *chn)
{
	struct sourceDescription *sd;
	
	if (strlen(serial) > 8) {
		return -1;
	}

	sd = new struct sourceDescription;
	if (!sd)
		return -1;

	sprintf(sd->name, "%s:%02X", serial, chn_id);
	sd->q_name = QString::fromAscii(sd->name);
	printf("Registering source '%s'\n", sd->name);

	sd->td = td;
	sd->chn_id = chn_id;

	sd->chipShortString = QString::fromAscii(chipToShortString(chn->chip_id));
	sd->chipString = QString::fromAscii(chipToString(chn->chip_id));
	sd->chn_data = chn;
	
	sourceList.append(sd);

	return 0;
}

int tenkiglue_syncDashboard(TenkiDashboard *td)
{
	// eventually add and remove. for now this just
	// blindly adds everything.
	if (!dash_list.isEmpty()) {
		return 0; 
	}

	for (int i=0; i<device_list.size(); i++) {
		DashSensor *ds;

		ds = new DashSensor(device_list.at(i));

		dash_list.append(ds);
		td->addDashSensor(ds);
	}

	return 0;
}

int tenkiglue_shutdown()
{
	while (!device_list.isEmpty()) {
		TenkiDevice *td;

		td = device_list.takeLast();

		delete td;
	}
	return 0;
}

