#include "CreateLogger.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <stdio.h>
#include "TenkiGlue.h"

CreateLogger::CreateLogger()
{
	main_layout = new QVBoxLayout();
	setLayout(main_layout);

	/* Source channels selection */
	sourcebox = new QGroupBox(tr("Data sources"));
	svb = new QVBoxLayout();
	sourcebox->setLayout(svb);
	tenkiglue_populateSourceCheckboxes(svb);
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
	comb_fmt->addItem(tr("Comma separated values - 00.00,11.11,..."));
	comb_fmt->addItem(tr("Tab separated values - 00.00(tab)11.11(tab)..."));
	comb_fmt->addItem(tr("Space separated values - 00.00 11.11 ..."));
	comb_fmt->addItem(tr("Semicolon separated values - 00.00;11.11;..."));
	
	dbl->addWidget(new QLabel(tr("Output file:")), 1, 0 );
	path = new QLineEdit();
	browseButton = new QPushButton(("..."));
	dbl->addWidget(path, 1, 1, 1, 3);
	dbl->addWidget(browseButton, 1, 4, 1, 1);

	dbl->addWidget(new QLabel(tr("Logging interval:")), 2, 0 );
	log_interval = new QLineEdit("10");
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
	printf("haha\n");


	QDialog::accept();
}

CreateLogger::~CreateLogger()
{
}

