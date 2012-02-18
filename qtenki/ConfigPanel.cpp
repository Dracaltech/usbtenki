#include "ConfigPanel.h"
#include <QDebug>
#include <QSettings>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QCheckBox>

#include "SelectableColor.h"
#include "ConfigPanel.h"
#include "globals.h"

ConfigPanel::ConfigPanel()
{
	QSettings settings;
	QVBoxLayout *lay = new QVBoxLayout();
	setLayout(lay);

	QGroupBox *dataBox = new QGroupBox(tr("Data processing and formatting"));
	QGridLayout *dataBox_layout = new QGridLayout();
	dataBox->setLayout(dataBox_layout);

	cb_disable_heat_index_validation = new QCheckBox(tr("Disable heat index input range check (may produce inaccurate values)"));
	cb_disable_heat_index_validation->setChecked(settings.value("data/disable_heat_index_range").toBool());
	cb_disable_humidex_validation = new QCheckBox(tr("Disable humidex input range check (may produce inaccurate values)"));
	cb_disable_humidex_validation->setChecked(settings.value("data/disable_heat_index_range").toBool());

	dataBox_layout->addWidget(cb_disable_heat_index_validation);
	dataBox_layout->addWidget(cb_disable_humidex_validation);	
	connect(cb_disable_heat_index_validation, SIGNAL(stateChanged(int)), this, SLOT(updateFlagsFromCheckboxes(int)));
	connect(cb_disable_humidex_validation, SIGNAL(stateChanged(int)), this, SLOT(updateFlagsFromCheckboxes(int)));

	default_palette = QApplication::palette();

	QGroupBox *appearanceBox = new QGroupBox(tr("Appearance"));
	QGridLayout *appBox_layout = new QGridLayout();
	appearanceBox->setLayout(appBox_layout);

	QColor def_win_color = QApplication::palette().color(QPalette::Active,QPalette::Window);
	sys_win_color = new SelectableColor("config/ovr_win_color", tr("Customize window color"), def_win_color);
	appBox_layout->addWidget(sys_win_color);	

	QColor def_btn_color = QApplication::palette().color(QPalette::Active,QPalette::Button);
	sys_btn_color = new SelectableColor("config/ovr_btn_color", tr("Customize button color"), def_btn_color);
	appBox_layout->addWidget(sys_btn_color);	

	QColor def_base_color = QApplication::palette().color(QPalette::Active,QPalette::Base);
	sys_base_color = new SelectableColor("config/ovr_base_color", tr("Customize base color"), def_base_color);
	appBox_layout->addWidget(sys_base_color);	

	appBox_layout->addWidget(new QLabel(tr("Note: The application may need to be restarted for appearance changes to take full effect.<br>Depending on your OS and/or theme, it might not be possible to change all colors.")));


	connect(sys_win_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_win_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));
	
	connect(sys_btn_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_btn_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));
	
	connect(sys_base_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_base_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));

	lay->addWidget(dataBox);	
	lay->addWidget(appearanceBox);	
	lay->addStretch();

	if (sys_win_color->getSelected()) {
		customColor(sys_win_color->getName(), sys_win_color->getColor());
	}
	if (sys_btn_color->getSelected()) {
		customColor(sys_btn_color->getName(), sys_btn_color->getColor());
	}
	if (sys_base_color->getSelected()) {
		customColor(sys_base_color->getName(), sys_base_color->getColor());
	}

}

void ConfigPanel::updateFlagsFromCheckboxes(int ignored)
{
	long flags = 0;

	if (cb_disable_heat_index_validation->isChecked())
		flags |= USBTENKI_FLAG_NO_HEAT_INDEX_RANGE;
	if (cb_disable_humidex_validation->isChecked())
		flags |= USBTENKI_FLAG_NO_HEAT_INDEX_RANGE;

	g_usbtenki_flags = flags;

}

void ConfigPanel::selectedChanged(QString name, int state, QColor color)
{
	if (!state) {
		defaultColor(name);
	} else {
		customColor(name, color);
	}
}

void ConfigPanel::defaultColor(QString name)
{
	QPalette myPalette;

	if (name == "config/ovr_win_color") {
		myPalette.setColor(QPalette::Active, QPalette::Window, default_palette.color(QPalette::Active, QPalette::Window));
		myPalette.setColor(QPalette::Inactive, QPalette::Window, default_palette.color(QPalette::Inactive, QPalette::Window));
		myPalette.setColor(QPalette::Disabled, QPalette::Window, default_palette.color(QPalette::Disabled, QPalette::Window));
	}

	if (name == "config/ovr_btn_color") {
		myPalette.setColor(QPalette::Active, QPalette::Button, default_palette.color(QPalette::Active, QPalette::Button));
		myPalette.setColor(QPalette::Inactive, QPalette::Button, default_palette.color(QPalette::Inactive, QPalette::Button));
		myPalette.setColor(QPalette::Disabled, QPalette::Button, default_palette.color(QPalette::Disabled, QPalette::Button));
	}

	if (name == "config/ovr_base_color") {
		myPalette.setColor(QPalette::Active, QPalette::Base, default_palette.color(QPalette::Active, QPalette::Base));
		myPalette.setColor(QPalette::Inactive, QPalette::Base, default_palette.color(QPalette::Inactive, QPalette::Base));
		myPalette.setColor(QPalette::Disabled, QPalette::Base, default_palette.color(QPalette::Disabled, QPalette::Base));
	}

	qApp->setPalette(myPalette);
	update();
}

void ConfigPanel::customColor(QString name, QColor col)
{
	if (name == "config/ovr_win_color" && sys_win_color->getSelected()) 
	{
		QPalette myPalette;

		myPalette.setColor(QPalette::Active, QPalette::Window, col);
		myPalette.setColor(QPalette::Inactive, QPalette::Window, col);
		myPalette.setColor(QPalette::Disabled, QPalette::Window, col);

		qApp->setPalette(myPalette);
	}

	if (name == "config/ovr_btn_color" && sys_btn_color->getSelected()) 
	{
		QPalette myPalette;

		myPalette.setColor(QPalette::Active, QPalette::Button, col);
		myPalette.setColor(QPalette::Inactive, QPalette::Button, col);
		myPalette.setColor(QPalette::Disabled, QPalette::Button, col);

		qApp->setPalette(myPalette);
	}

	if (name == "config/ovr_base_color" && sys_base_color->getSelected()) 
	{
		QPalette myPalette;

		myPalette.setColor(QPalette::Active, QPalette::Base, col);
		myPalette.setColor(QPalette::Inactive, QPalette::Base, col);
		myPalette.setColor(QPalette::Disabled, QPalette::Base, col);

		qApp->setPalette(myPalette);
	}


}

ConfigPanel::~ConfigPanel()
{
}


