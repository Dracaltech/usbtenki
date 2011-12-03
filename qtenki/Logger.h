#ifndef _logger_h__
#define _logger_h__

#include <QtGui>
#include <QWidget>
#include "TenkiSources.h"
#include "DataSourceCheckBox.h"

class Logger : public QWidget, public TenkiSourceAddRemove
{
	Q_OBJECT

	public:
		Logger(TenkiSources *s);
		~Logger();

		void addTenkiSource(struct sourceDescription *sd);
		void removeTenkiSource(struct sourceDescription *sd);

	public slots:
		void browse_clicked();

	private:
		TenkiSources *tenkisources;

		QGroupBox *sourcebox;
		QVBoxLayout *svb;
		QList<DataSourceCheckBox*> sources;

		QGroupBox *destbox;
		QGridLayout *dbl;
		QComboBox *comb_fmt;
		QLineEdit *path;
		QPushButton *browseButton;
		
		QSpinBox *log_interval;		

		QWidget *mid_layer; // source and dest
		QHBoxLayout *mid_layout;

		QGroupBox *control;
		QHBoxLayout *control_layout;
		QPushButton *start_button, *stop_button;
		QLabel *status_label;
	

		QGroupBox *messages;


		QVBoxLayout *main_layout;

};

#endif // _logger_h__

