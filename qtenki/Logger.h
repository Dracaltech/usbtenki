#ifndef _logger_h__
#define _logger_h__

#include <QtGui>
#include <QWidget>
#include "TenkiSources.h"
#include "DataSourceCheckBox.h"
#include "SimpleLogger.h"

class Logger : public QWidget, public TenkiSourceAddRemove
{
	Q_OBJECT

	public:
		Logger(TenkiSources *s);
		~Logger();

		void addTenkiSource(struct sourceDescription *sd);
		void removeTenkiSource(struct sourceDescription *sd);

	public slots:
		void openViewer();
		void browse_clicked();
		void startLogging();
		void stopLogging();

	protected:
		void logMessage(QString str);
		void cannotStartPopup(QString reason, QString hint);

	protected slots:
		void loggerStarted();
		void loggerStopped();
		void loggerMessage(QString str);
		void loggerActivity(int counter);

	private:
		TenkiSources *tenkisources;

		QGroupBox *sourcebox;
		QVBoxLayout *svb;
		QList<DataSourceCheckBox*> sources;

		QGroupBox *destbox;
		QGridLayout *dbl;
		QComboBox *comb_fmt;
		QLineEdit *path;
		QPushButton *browseButton, *viewButton;
		
		QSpinBox *log_interval;		

		QWidget *mid_layer; // source and dest
		QHBoxLayout *mid_layout;

		QGroupBox *control;
		QHBoxLayout *control_layout;
		QPushButton *start_button, *stop_button;
		QLabel *status_label, *counter_label;
	
		QGroupBox *messages;
		QHBoxLayout *msg_layout;
		QTextEdit *msgtxt;
			
		QVBoxLayout *main_layout;

		SimpleLogger *current_logger;
};

#endif // _logger_h__

