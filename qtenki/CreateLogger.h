#ifndef _create_logger_h__
#define _create_logger_h__

#include <QtGui>
#include "DataSourceCheckBox.h"
#include "TenkiSources.h"

class CreateLogger : public QDialog
{
	Q_OBJECT

	public:
		CreateLogger(TenkiSources *s);
		~CreateLogger();
	
	public slots:
		void accept();
		void browse_clicked();

	private:
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

		QVBoxLayout *main_layout;
		QPushButton *btn_create;
		QPushButton *btn_cancel;
		QDialogButtonBox *btnbox;
};

#endif // _create_logger_h__
