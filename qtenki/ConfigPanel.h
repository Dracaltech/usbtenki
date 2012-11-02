#ifndef _ConfigPanel_h__
#define _ConfigPanel_h__

#include <QWidget>
#include <QString>
#include <QColor>
#include <QPalette>
#include <QLabel>

#include "SelectableColor.h"

class ConfigPanel : public QWidget
{
	Q_OBJECT

	public:
		ConfigPanel();
		~ConfigPanel();

	public slots:
		void setEnabled(bool b);

	private slots:
		void defaultColor(QString name);
		void customColor(QString name, QColor col);
		void selectedChanged(QString name, int state, QColor col);
		void updateFlagsFromCheckboxes(int);
		void updateMinimizeToTray(int);

	private:
		void displaySaved();
		QPalette default_palette;
		SelectableColor *sys_win_color;
		SelectableColor *sys_btn_color;
		SelectableColor *sys_base_color;
		QCheckBox *cb_use_old_sht_coefficients;
		QCheckBox *cb_disable_heat_index_validation;
		QCheckBox *cb_disable_humidex_validation;
		QCheckBox *cb_minimize_to_system_tray;
		QLabel *messageLabel;

};

#endif // _ConfigPanel_h__
