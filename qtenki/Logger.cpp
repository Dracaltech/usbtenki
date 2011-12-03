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
	path = new QLineEdit();
	browseButton = new QPushButton(("..."));
	dbl->addWidget(path, 1, 1, 1, 3);
	dbl->addWidget(browseButton, 1, 4, 1, 1);


	QObject::connect(browseButton, SIGNAL(clicked()), this, SLOT(browse_clicked()));

	dbl->addWidget(new QLabel(tr("Logging interval:")), 2, 0 );
	log_interval = new QSpinBox();
	log_interval->setMinimum(1);
	dbl->addWidget(log_interval, 2, 1, 1, 2);
	dbl->addWidget(new QLabel(tr("(seconds)")), 2, 4 );


	control = new QGroupBox("Control");
	control_layout = new QHBoxLayout();
	control->setLayout(control_layout);
	start_button = new QPushButton("Start");
	stop_button = new QPushButton("Stop");
	status_label = new QLabel("Not running");
	control_layout->addWidget(start_button);
	control_layout->addWidget(stop_button);
	control_layout->addWidget(status_label);
	control_layout->addStretch();
	
	messages = new QGroupBox("Messages");

/*
	btn_create = new QPushButton("Create");
	btn_cancel = new QPushButton("Cancel");

	btnbox = new QDialogButtonBox();		
	btnbox->addButton(btn_create, QDialogButtonBox::AcceptRole);
	btnbox->addButton(btn_cancel, QDialogButtonBox::RejectRole);


	QObject::connect(btnbox, SIGNAL(rejected()), this, SLOT(reject()));
	QObject::connect(btnbox, SIGNAL(accepted()), this, SLOT(accept()));
*/
	
	/* Layout */
	mid_layer = new QWidget();
	mid_layout = new QHBoxLayout();
	mid_layer->setLayout(mid_layout);
	mid_layout->addWidget(sourcebox);
	mid_layout->addWidget(destbox);

	main_layout->addWidget(mid_layer);	

	main_layout->addWidget(control);
	main_layout->addWidget(messages);

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

void Logger::browse_clicked()
{
	QString filename;

	
	filename = QFileDialog::getSaveFileName(this, tr("Output file"), "", "(*.txt *.csv *.tsv)" );

	path->setText(filename);
}

