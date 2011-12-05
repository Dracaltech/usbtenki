#include "Logger.h"


Logger::Logger(TenkiSources *s)
{
	tenkisources = s;

	main_layout = new QVBoxLayout();
	setLayout(main_layout);

	/* Source channels selection */
	sourcebox = new QGroupBox(tr("Data sources"));
	svb = new QVBoxLayout();
	sourcebox->setLayout(svb);
	
	//tenkiglue_populateSourceCheckboxes(svb, &sources);
	tenkisources->addSourcesTo(this);
	svb->addStretch();

	/* Output selection */
	destbox = new QGroupBox(tr("Output configuration"));
	dbl = new QGridLayout();
	destbox->setLayout(dbl);

	comb_fmt = new QComboBox();
	dbl->addWidget(new QLabel(tr("File format:")), 0, 0 );
	dbl->addWidget(comb_fmt, 0, 1);
	/* Note: Added in this order to match SimpleLogger::FileFormat */
	comb_fmt->addItem(tr("Comma separated values - 00.00,11.11,..."));
	comb_fmt->addItem(tr("Tab separated values - 00.00(tab)11.11(tab)..."));
	comb_fmt->addItem(tr("Space separated values - 00.00 11.11 ..."));
	comb_fmt->addItem(tr("Semicolon separated values - 00.00;11.11;..."));
	
	dbl->addWidget(new QLabel(tr("Output file:")), 1, 0 );
	path = new QLineEdit("tenkilog.txt");
	browseButton = new QPushButton(("..."));
	dbl->addWidget(path, 1, 1, 1, 3);
	dbl->addWidget(browseButton, 1, 4, 1, 1);


	QObject::connect(browseButton, SIGNAL(clicked()), this, SLOT(browse_clicked()));

	dbl->addWidget(new QLabel(tr("Logging interval:")), 2, 0 );
	log_interval = new QSpinBox();
	log_interval->setMinimum(1);
	dbl->addWidget(log_interval, 2, 1, 1, 1);
	dbl->addWidget(new QLabel(tr("(seconds)")), 2, 3 );


	control = new QGroupBox("Control");
	control_layout = new QHBoxLayout();
	control->setLayout(control_layout);
	start_button = new QPushButton("Start");
	stop_button = new QPushButton("Stop");
	stop_button->setEnabled(false);
	status_label = new QLabel("Not running");
	control_layout->addWidget(start_button);
	control_layout->addWidget(stop_button);
	control_layout->addWidget(status_label);
	control_layout->addStretch();
	
	messages = new QGroupBox("Messages");
	msg_layout = new QHBoxLayout();
	messages->setLayout(msg_layout);
	msgtxt = new QTextEdit();
	msgtxt->setReadOnly(true);
	
	msg_layout->addWidget(msgtxt);

	QObject::connect(start_button, SIGNAL(clicked()), this, SLOT(startLogging()));
	QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stopLogging()));

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
		case 3: fmt = SimpleLogger::Scsv; break;
	}
	
	current_logger = new SimpleLogger(tenkisources, path->text(), log_interval->value(), fmt);

	for (int i=0; i<sources.size(); i++) {
		DataSourceCheckBox *cb = sources.at(i);
		if (cb->isChecked()) {
			current_logger->addSource(cb->src);
		}
	}

	start_button->setEnabled(false);
	stop_button->setEnabled(true);
	mid_layer->setEnabled(false);
	logMessage("Starting logger...");

	QObject::connect(current_logger, SIGNAL(started()), this, SLOT(loggerStarted()));	
	QObject::connect(current_logger, SIGNAL(finished()), this, SLOT(loggerStopped()));	
	QObject::connect(current_logger, SIGNAL(logMessage(QString)), this, SLOT(loggerMessage(QString)));
	current_logger->start();	
}

void Logger::stopLogging()
{
	logMessage("Stopping logger...");
	current_logger->quit();
	logMessage("Waiting for logger thread to stop...");
	current_logger->wait();
}

void Logger::browse_clicked()
{
	QString filename;

	
	filename = QFileDialog::getSaveFileName(this, tr("Output file"), "", "(*.txt *.csv *.tsv)" );

	path->setText(filename);
}

void Logger::loggerStarted()
{
	logMessage("logger started successfully");
}

void Logger::loggerStopped()
{
	logMessage("logger stopped successfully");

	start_button->setEnabled(true);
	stop_button->setEnabled(false);
	mid_layer->setEnabled(true);

	delete current_logger;
}

