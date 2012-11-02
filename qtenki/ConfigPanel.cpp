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
#include "ConfigCheckbox.h"
#include "TemperaturePreference.h"
#include "FrequencyPreference.h"
#include "PressurePreference.h"

ConfigPanel::ConfigPanel()
{
	QSettings settings;
	QVBoxLayout *lay = new QVBoxLayout();
	setLayout(lay);

	///////////////////
	QGroupBox *dataBox = new QGroupBox(tr("Data processing and formatting"));
	QGridLayout *dataBox_layout = new QGridLayout();
	dataBox->setLayout(dataBox_layout);

	cb_use_old_sht_coefficients = new QCheckBox(tr("Use old SHT75 relative humidity non-linearity correction coefficients (Datasheet rev.3, 2007)"));
	cb_use_old_sht_coefficients->setChecked(settings.value("data/use_old_coefficients").toBool());
	cb_disable_heat_index_validation = new QCheckBox(tr("Disable heat index input range check (may produce inaccurate values)"));
	cb_disable_heat_index_validation->setChecked(settings.value("data/disable_heat_index_range").toBool());
	cb_disable_humidex_validation = new QCheckBox(tr("Disable humidex input range check (may produce inaccurate values)"));
	cb_disable_humidex_validation->setChecked(settings.value("data/disable_humidex_range").toBool());
	TemperaturePreference *t_pref = new TemperaturePreference();
	PressurePreference *p_pref = new PressurePreference();
	FrequencyPreference *f_pref = new FrequencyPreference();
	
	dataBox_layout->addWidget(new QLabel(tr("Temperature unit: ")), 0, 0 );
	dataBox_layout->addWidget(t_pref, 0, 1);
	
	dataBox_layout->addWidget(new QLabel(tr("Pressure unit: ")), 1, 0 );
	dataBox_layout->addWidget(p_pref, 1, 1);
	
	dataBox_layout->addWidget(new QLabel(tr("Frequency unit: ")), 2, 0 );
	dataBox_layout->addWidget(f_pref, 2, 1);


	dataBox_layout->addWidget(cb_use_old_sht_coefficients, 3, 0, 1, -1);
	dataBox_layout->addWidget(cb_disable_heat_index_validation, 4, 0, 1, -1);
	dataBox_layout->addWidget(cb_disable_humidex_validation, 5, 0, 1, -1);	

	dataBox_layout->setColumnStretch(2, 100);
	
	connect(cb_use_old_sht_coefficients, SIGNAL(stateChanged(int)), this, SLOT(updateFlagsFromCheckboxes(int)));
	connect(cb_disable_heat_index_validation, SIGNAL(stateChanged(int)), this, SLOT(updateFlagsFromCheckboxes(int)));
	connect(cb_disable_humidex_validation, SIGNAL(stateChanged(int)), this, SLOT(updateFlagsFromCheckboxes(int)));


	///////////////////////////
	default_palette = QApplication::palette();

	QGroupBox *appearanceBox = new QGroupBox(tr("Appearance"));
	QGridLayout *appBox_layout = new QGridLayout();
	appearanceBox->setLayout(appBox_layout);

	ConfigCheckbox *cb_bv_aliases = new ConfigCheckbox(tr("Show aliases in big view"), "bigview/show_aliases");
	appBox_layout->addWidget(cb_bv_aliases);
	ConfigCheckbox *cb_bv_units = new ConfigCheckbox(tr("Show units in big view"), "bigview/show_units");
	appBox_layout->addWidget(cb_bv_units);

	cb_minimize_to_system_tray = new QCheckBox(tr("Minimize window to system tray"));
	cb_minimize_to_system_tray->setChecked(settings.value("ui/minimize_to_tray").toBool());
	appBox_layout->addWidget(cb_minimize_to_system_tray);
	connect(cb_minimize_to_system_tray, SIGNAL(stateChanged(int)), this, SLOT(updateMinimizeToTray(int)));

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

	messageLabel = new QLabel("<img src=':/attention.png'><b>Configuration cannot be changed while logging.</b>");
	lay->addWidget(messageLabel);
	messageLabel->setVisible(false);

	lay->addWidget(dataBox);	
	lay->addWidget(appearanceBox);	
	lay->addStretch();

	lay->addWidget(new QLabel("Configuration changes are saved automatically and immediately effective."));


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

void ConfigPanel::setEnabled(bool b)
{
	messageLabel->setVisible(!b);
	QWidget::setEnabled(b);
}

void ConfigPanel::updateFlagsFromCheckboxes(int ignored)
{
	QSettings settings;
	long flags = 0;

	if (cb_disable_heat_index_validation->isChecked()) {
		flags |= USBTENKI_FLAG_NO_HEAT_INDEX_RANGE;
	}

	if (cb_disable_humidex_validation->isChecked()) {
		flags |= USBTENKI_FLAG_NO_HUMIDEX_RANGE;
	}

	if (cb_use_old_sht_coefficients->isChecked()) {
		flags |= USBTENKI_FLAG_USE_OLD_SHT75_COMPENSATION;
	}

	g_usbtenki_flags = flags;

	settings.setValue("data/disable_heat_index_range", cb_disable_heat_index_validation->isChecked());
	settings.setValue("data/disable_humidex_range", cb_disable_humidex_validation->isChecked());
	settings.setValue("data/use_old_coefficients", cb_use_old_sht_coefficients->isChecked());
}

void ConfigPanel::updateMinimizeToTray(int ig)
{
	QSettings settings;

	minimize_to_tray = cb_minimize_to_system_tray->isChecked();

	settings.setValue("ui/minimize_to_tray", cb_minimize_to_system_tray->isChecked());
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


