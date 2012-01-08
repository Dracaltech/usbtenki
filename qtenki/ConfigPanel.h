#ifndef _ConfigPanel_h__
#define _ConfigPanel_h__

#include <QWidget>
#include <QString>
#include <QColor>
#include <QPalette>

#include "SelectableColor.h"

class ConfigPanel : public QWidget
{
	Q_OBJECT

	public:
		ConfigPanel();
		~ConfigPanel();

	private slots:
		void defaultColor(QString name);
		void customColor(QString name, QColor col);
		void selectedChanged(QString name, int state, QColor col);

	private:
		QPalette default_palette;
		SelectableColor *sys_win_color;
		SelectableColor *sys_btn_color;
		SelectableColor *sys_base_color;
};

#endif // _ConfigPanel_h__
