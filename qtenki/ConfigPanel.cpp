#include "ConfigPanel.h"
#include <QDebug>
#include <QSettings>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>

#include "SelectableColor.h"
#include "ConfigPanel.h"

ConfigPanel::ConfigPanel()
{
	QSettings settings;
	QVBoxLayout *lay = new QVBoxLayout();
	setLayout(lay);

	default_palette = QApplication::palette();

	QGroupBox *appearanceBox = new QGroupBox(tr("Apparence"));
	QGridLayout *appBox_layout = new QGridLayout();
	appearanceBox->setLayout(appBox_layout);

	QColor def_win_color = QApplication::palette().color(QPalette::Active,QPalette::Window);
	sys_win_color = new SelectableColor("config/ovr_win_color", tr("Customize window color"), def_win_color);
	appBox_layout->addWidget(sys_win_color);	

	QColor def_btn_color = QApplication::palette().color(QPalette::Active,QPalette::Button);
	sys_btn_color = new SelectableColor("config/ovr_btn_color", tr("Customize button color"), def_btn_color);
	appBox_layout->addWidget(sys_btn_color);	

	QColor def_base_color = QApplication::palette().color(QPalette::Active,QPalette::Base);
	sys_base_color = new SelectableColor("config/ovr_base_color", tr("Customize button color"), def_base_color);
	appBox_layout->addWidget(sys_base_color);	


	connect(sys_win_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_win_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));
	
	connect(sys_btn_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_btn_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));
	
	connect(sys_base_color, SIGNAL(colorChanged(QString,QColor)), this, SLOT(customColor(QString,QColor))); 	
	connect(sys_base_color, SIGNAL(selectedChanged(QString,int,QColor)), this, SLOT(selectedChanged(QString,int,QColor)));
	
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

	if (name == "config/ovr_base_color" && sys_btn_color->getSelected()) 
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


