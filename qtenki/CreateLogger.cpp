#include "CreateLogger.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>

#include <stdio.h>
#include "TenkiGlue.h"
#include "SimpleLogger.h"

CreateLogger::CreateLogger()
{
	main_layout = new QVBoxLayout();
	setLayout(main_layout);

	/* Source channels selection */
	sourcebox = new QGroupBox(tr("Data sources"));
	svb = new QVBoxLayout();
	sourcebox->setLayout(svb);
	tenkiglue_populateSourceCheckboxes(svb, &sources);
/*		
	QCheckBox *qcb = new QCheckBox("Test");
	sources.append(qcb);
	svb->addWidget(qcb);

	qcb = new QCheckBox("Test");
	sources.append(qcb);
	svb->addWidget(qcb);
*/

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

	

	/* Create / Cancel */
	btn_create = new QPushButton("Create");
	btn_cancel = new QPushButton("Cancel");

	btnbox = new QDialogButtonBox();		
	btnbox->addButton(btn_create, QDialogButtonBox::AcceptRole);
	btnbox->addButton(btn_cancel, QDialogButtonBox::RejectRole);


	QObject::connect(btnbox, SIGNAL(rejected()), this, SLOT(reject()));
	QObject::connect(btnbox, SIGNAL(accepted()), this, SLOT(accept()));

	
	/* Layout */
	mid_layer = new QWidget();
	mid_layout = new QHBoxLayout();
	mid_layer->setLayout(mid_layout);
	mid_layout->addWidget(sourcebox);
	mid_layout->addWidget(destbox);

	main_layout->addWidget(mid_layer);	
	main_layout->addWidget(btnbox);
}

void CreateLogger::accept()
{
	SimpleLogger *logger;

	/* Make sure a file was specified.
	 * TODO: Test for write access? */
	if (0 == path->text().size()) {
		QMessageBox msgBox;

		msgBox.setText(tr("No file selected."));
		msgBox.setInformativeText(tr("Select a file or click cancel."));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
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
	

	logger = new SimpleLogger(path->text(), log_interval->value(), fmt);

	for (int i=0; i<sources.size(); i++) {
		DataSourceCheckBox *cb = sources.at(i);
	
		if (cb->isChecked()) {
			logger->addSource(cb->src);
			qDebug() << cb->src;

		}
	}

	//
	logger->start();
	//

	QDialog::accept();
}

void CreateLogger::browse_clicked()
{
	QString filename;

	filename = QFileDialog::getSaveFileName(this, tr("Output file"), "", "(*.txt *.csv *.tsv)" );

	path->setText(filename);
}

CreateLogger::~CreateLogger()
{
}

