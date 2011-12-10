#include "DataSourceCheckBox.h"
#include <QHBoxLayout>
#include <QLabel>

DataSourceCheckBox::DataSourceCheckBox(QString caption, QString src)
{
	checkbox = new QCheckBox();
	checkbox->setText(caption);

	alias_edit = new QLineEdit();
	
	this->src = src;

	QHBoxLayout *lay = new QHBoxLayout();
	setLayout(lay);

	lay->addWidget(checkbox);
	lay->addStretch();
	lay->addWidget(new QLabel(tr("Alias:")));
	lay->addWidget(alias_edit);

	alias_edit->setMinimumWidth(150);
	alias_edit->setMaximumWidth(200);

}

bool DataSourceCheckBox::isChecked()
{
	return checkbox->isChecked();
}

DataSourceCheckBox::~DataSourceCheckBox()
{
}
