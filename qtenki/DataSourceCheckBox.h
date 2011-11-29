#ifndef _DataSourceCheckBox
#define _DataSourceCheckBox

#include <QString>
#include <QCheckBox>

class DataSourceCheckBox : public QCheckBox
{
	Q_OBJECT

	public:
		DataSourceCheckBox(QString caption, QString src);
		~DataSourceCheckBox();
		QString src;	
};

#endif

