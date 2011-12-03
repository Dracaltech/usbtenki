#ifndef _loggespanel_h__
#define _loggespanel_h__

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QString>
#include <QList>
#include "TenkiSources.h"
#include "TenkiDevice.h"

class LoggersPanel : public QWidget
{
	Q_OBJECT

	public:
		LoggersPanel(TenkiSources *s);
		~LoggersPanel();

	private slots:
		void openCreator();

	private:
		void addLogger();
		QWidget listArea;
		QString title;
		QGridLayout *listLayout;
		QVBoxLayout *mainLayout;
		QList<QLabel*> values;
		TenkiSources *tenkisources;
};

#endif

