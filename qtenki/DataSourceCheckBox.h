#ifndef _DataSourceCheckBox
#define _DataSourceCheckBox

#include <QString>
#include <QCheckBox>
#include <QLineEdit>

class DataSourceCheckBox : public QWidget
{
	Q_OBJECT

	public:
		DataSourceCheckBox(QString caption, QString src);
		~DataSourceCheckBox();
		QString src;	

		bool isChecked();

	private:
		QCheckBox *checkbox;
		QLineEdit *alias_edit;
};

#endif

