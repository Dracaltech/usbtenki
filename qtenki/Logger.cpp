#include "Logger.h"
#include "TextViewer.h"

Logger::Logger(TenkiSources *s)
{
	int y=0;
	QSettings settings;
	QLocale locale;

	tenkisources = s;

	main_layout = new QVBoxLayout();
	setLayout(main_layout);

	/* Source channels selection */
	sourcebox = new QGroupBox(tr("Data sources"));
	svb = new QVBoxLayout();
	sourcebox->setLayout(svb);
	
	// have tenkisource call our addTenkiSource in loop for
	// each source present.
	tenkisources->addSourcesTo(this);
	svb->addStretch();

	/* Output selection */
	destbox = new QGroupBox(tr("Output configuration"));
	dbl = new QGridLayout();
	destbox->setLayout(dbl);

	comb_fmt = new QComboBox();
	/* Note: Added in this order to match SimpleLogger::FileFormat */
	comb_fmt->addItem(tr("Comma"));
	comb_fmt->addItem(tr("Tab"));
	comb_fmt->addItem(tr("Space"));
	comb_fmt->addItem(tr("Semicolon"));
	comb_fmt->setCurrentIndex(settings.value("logger/format").toInt());
	connect(comb_fmt, SIGNAL(currentIndexChanged(int)), this, SLOT(logFormatChanged(int)));

	dbl->addWidget(new QLabel(tr("Field separator:")), y, 0 );
	dbl->addWidget(comb_fmt, y, 1);
//	y++;
	
	comb_decimal = new QComboBox();
	/* Note: Added in order matching SimpleLogger::DecimalType */
	comb_decimal->addItem(tr("System default"));
	comb_decimal->addItem(tr("Period: ")+" .");
	comb_decimal->addItem(tr("Comma: ")+" ,");
	comb_decimal->setCurrentIndex(settings.value("logger/decimal_point").toInt());
	connect(comb_decimal, SIGNAL(currentIndexChanged(int)), this, SLOT(decimalPointChanged(int)));
	dbl->addWidget(new QLabel(tr("Decimal point:")), y, 2 );
	dbl->addWidget(comb_decimal, y, 3);
	y++;

	comb_timestamp = new QComboBox();
	/* Note: Added in order matching SimpleLogger::TimeStampFormat */
	comb_timestamp->addItem(tr("None"));
	comb_timestamp->addItem(tr("Short system-specific format"));
	comb_timestamp->addItem(tr("Long system-specific format"));
	comb_timestamp->addItem(tr("ISO8601 (single field YYYY-MM-DDTHH:MM:SS)"));
	comb_timestamp->addItem(tr("ISO8601 (date YYYY-MM-DD and hour HH:MM:SS across two fields)"));
	comb_timestamp->addItem(tr("ISO8601 Time only (HH:MM:SS)"));


	comb_timestamp->setCurrentIndex(settings.value("logger/timestamp").toInt());
	connect(comb_timestamp, SIGNAL(currentIndexChanged(int)), this, SLOT(timestampChanged(int)));
	dbl->addWidget(new QLabel(tr("Timestamps:")), y, 0 );
	dbl->addWidget(comb_timestamp, y, 1, 1, 4);
	y++;

	
	dbl->addWidget(new QLabel(tr("Output file:")), y, 0 );
	path = new QLineEdit(settings.value("logger/filename").toString());
	browseButton = new QPushButton(tr("Select"));
	viewButton = new QPushButton(tr("View"));
	dbl->addWidget(path, y, 1, 1, 4);
	y++;

	dbl->addWidget(browseButton, y, 3, 1, 1);
	dbl->addWidget(viewButton, y, 4, 1, 1);
	y++;

	connect(path, SIGNAL(editingFinished()), this, SLOT(filenameEdited()));

	QObject::connect(browseButton, SIGNAL(clicked()), this, SLOT(browse_clicked()));
	QObject::connect(viewButton, SIGNAL(clicked()), this, SLOT(openViewer()));

	dbl->addWidget(new QLabel(tr("Logging interval:")), y, 0 );
	log_interval = new QSpinBox();
	log_interval->setMinimum(1);
	dbl->addWidget(log_interval, y, 1, 1, 1);
	dbl->addWidget(new QLabel(tr("(seconds)")), y, 2 );
	
	log_interval->setValue(settings.value("logger/interval").toInt());
	connect(log_interval, SIGNAL(valueChanged(int)), this, SLOT(intervalChanged(int)));


	control = new QGroupBox(tr("Control"));
	control_layout = new QHBoxLayout();
	control->setLayout(control_layout);
	start_button = new QPushButton(tr("Start"));
	stop_button = new QPushButton(tr("Stop"));
	stop_button->setEnabled(false);
	status_label = new QLabel(tr("Not running."));
	counter_label = new QLabel("0");
	control_layout->addWidget(start_button);
	control_layout->addWidget(stop_button);
	control_layout->addWidget(status_label);
	control_layout->addWidget(new QLabel(tr("Lines written: ")));
	control_layout->addWidget(counter_label);
	control_layout->addStretch();
	
	messages = new QGroupBox("Messages");
	msg_layout = new QHBoxLayout();
	messages->setLayout(msg_layout);
	msgtxt = new QTextEdit();
	msgtxt->setReadOnly(true);
	
	msg_layout->addWidget(msgtxt);

	connect(start_button, SIGNAL(clicked()), this, SLOT(startLogging()));
	connect(stop_button, SIGNAL(clicked()), this, SLOT(stopLogging()));

	/* Layout */
	mid_layer = new QWidget();
	mid_layout = new QHBoxLayout();
	mid_layer->setLayout(mid_layout);
	mid_layout->addWidget(sourcebox);
	mid_layout->addWidget(destbox);

	main_layout->addWidget(mid_layer);	

	main_layout->addWidget(control);
	main_layout->addWidget(messages);
	main_layout->setStretchFactor(messages, 100);

}

Logger::~Logger()
{
}

void Logger::addTenkiSource(struct sourceDescription *sd)
{
	 DataSourceCheckBox *cb = new DataSourceCheckBox(sd->q_name + "  --  " + sd->chipShortString, sd->q_name);
	 svb->addWidget(cb);
	 sources.append(cb);
}

void Logger::removeTenkiSource(struct sourceDescription *sd)
{
}

void Logger::logMessage(QString str)
{
	QDateTime now = QDateTime::currentDateTime();
	msgtxt->append(now.toString("(yyyy-MM-dd hh:mm:ss)") + " " + str);
}

void Logger::loggerMessage(QString str)
{
	logMessage("Logger: "+str);
}

void Logger::cannotStartPopup(QString reason, QString hint)
{
	QMessageBox msgBox;
	msgBox.setText(reason);
	msgBox.setInformativeText(hint);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.exec();
}

void Logger::startLogging()
{

	/* Make sure a file was specified.
	 * TODO: Test for write access? */

	if (0 == path->text().size()) {
		cannotStartPopup(tr("No file selected."), tr("You must select an output file."));
		return;
	}

	QFileInfo qf = QFileInfo(path->text());
	if (qf.isFile()) {
		logMessage("warning: File already exists");
		if (!qf.isWritable()) {
			cannotStartPopup(tr("File is not writeable."), tr("Select another file or check permissions."));
			return;
		}
	}
	
	if (comb_fmt->currentIndex() == -1) {
		QMessageBox msgBox;
		
		msgBox.setText(tr("No file format selected."));
		msgBox.setInformativeText(tr("Please select an output file format."));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	
	SimpleLogger::FileFormat fmt = SimpleLogger::Csv;
	
	switch(comb_fmt->currentIndex()) {
		case 0: fmt = SimpleLogger::Csv; break;
		case 1: fmt = SimpleLogger::Tsv; break;
		case 2: fmt = SimpleLogger::Ssv; break;
		case 3: fmt = SimpleLogger::Scsv;break;
	}

	SimpleLogger::DecimalType dt = SimpleLogger::SystemFormat;

	switch(comb_decimal->currentIndex()) {
		case 0: dt = SimpleLogger::SystemFormat;break;
		case 1: dt = SimpleLogger::Period;		break;
		case 2: dt = SimpleLogger::Comma;		break;
	}

	SimpleLogger::TimeStampFormat tfmt = SimpleLogger::None;
	switch(comb_timestamp->currentIndex()) {
		case 0: tfmt = SimpleLogger::None;				break;
		case 1: tfmt = SimpleLogger::SystemShort;		break;
		case 2: tfmt = SimpleLogger::SystemLong;		break;
		case 3: tfmt = SimpleLogger::ISO8601;			break;
		case 4: tfmt = SimpleLogger::SplitISO8601;		break;
		case 5: tfmt = SimpleLogger::ISO8601TimeOnly;	break;
	}

	current_logger = new SimpleLogger(tenkisources, path->text(), log_interval->value(), fmt, dt, tfmt);

	for (int i=0; i<sources.size(); i++) {
		DataSourceCheckBox *cb = sources.at(i);
		if (cb->isChecked()) {
			current_logger->addSource(cb->src, cb->getAlias());
		}
	}

	start_button->setEnabled(false);
	stop_button->setEnabled(true);
	mid_layer->setEnabled(false);
	logMessage("Starting logger...");

	QObject::connect(current_logger, SIGNAL(started()), this, SLOT(loggerStarted()));	
	QObject::connect(current_logger, SIGNAL(finished()), this, SLOT(loggerStopped()));	
	QObject::connect(current_logger, SIGNAL(logMessage(QString)), this, SLOT(loggerMessage(QString)));
	QObject::connect(current_logger, SIGNAL(logged(int)), this, SLOT(loggerActivity(int)));
	current_logger->start();
}

void Logger::stopLogging()
{
	logMessage("Stopping logger...");
	current_logger->quit();
	logMessage("Waiting for logger thread to stop...");
	status_label->setText(tr("Not running."));
	current_logger->wait();
}

void Logger::openViewer()
{
	TextViewer *t;
	
	t = new TextViewer(path->text());
	t->exec();
}

void Logger::browse_clicked()
{
	QString filename;

	
	filename = QFileDialog::getSaveFileName(this, tr("Output file"), "", "(*.txt *.csv *.tsv)" );

	path->setText(filename);
	filenameEdited(); // QLineEdit does not emit a signal in this case.
}

void Logger::loggerStarted()
{
	logMessage("logger started successfully");
	status_label->setText(tr("Running."));
}

void Logger::loggerActivity(int count)
{
	QString str;
	str.setNum(count);
	counter_label->setText(str);
}

void Logger::loggerStopped()
{
	logMessage("logger stopped successfully");

	start_button->setEnabled(true);
	stop_button->setEnabled(false);
	mid_layer->setEnabled(true);

	delete current_logger;
}

void Logger::timestampChanged(int idx)
{
	QSettings settings;
	settings.setValue("logger/timestamp", idx);
}

void Logger::decimalPointChanged(int idx)
{
	QSettings settings;
	settings.setValue("logger/decimal_point", idx);
}

void Logger::logFormatChanged(int idx)
{
	QSettings settings;
	settings.setValue("logger/format", idx);
}

void Logger::intervalChanged(int i)
{
	QSettings settings;
	settings.setValue("logger/interval", i);
}

void Logger::filenameEdited()
{
	QSettings settings;
	settings.setValue("logger/filename", path->text());
}
