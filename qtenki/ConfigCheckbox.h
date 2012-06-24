#ifndef _config_checkbox_h__
#define _config_checkbox_h__


#include <QtGui>

class ConfigCheckbox : public QCheckBox
{
	Q_OBJECT

	public:
		ConfigCheckbox(QString caption, QString config_key);

	private slots:
		void update(int state);

	private:
		QString config_key;

};

#endif // _config_checkbox_h__

