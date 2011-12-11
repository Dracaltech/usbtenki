#include "DataSourceCheckBox.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QSettings>

DataSourceCheckBox::DataSourceCheckBox(QString caption, QString src)
{
	QSettings settings;

	checkbox = new QCheckBox();
	checkbox->setText(caption);

	checkbox->setChecked(settings.value("sourcesChecked/"+src).toBool());

	alias_edit = new QLineEdit();
	alias_edit->setText(settings.value("sourcesAliases/"+src).toString());
	
	this->src = src;

	QHBoxLayout *lay = new QHBoxLayout();
	setLayout(lay);

	lay->addWidget(checkbox);
	lay->addStretch();
	lay->addWidget(new QLabel(tr("Alias:")));
	lay->addWidget(alias_edit);

	alias_edit->setMinimumWidth(150);
	alias_edit->setMaximumWidth(200);

	connect(alias_edit, SIGNAL(editingFinished()), this, SLOT(aliasChanged()));
	connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(checkChanged(int)));
}

bool DataSourceCheckBox::isChecked()
{
	return checkbox->isChecked();
}

void DataSourceCheckBox::checkChanged(int st)
{
	QSettings settings;
	settings.setValue("sourcesChecked/"+src, checkbox->isChecked());
}

void DataSourceCheckBox::aliasChanged()
{
	QSettings settings;
	settings.setValue("sourcesAliases/"+src, alias_edit->text());
}

DataSourceCheckBox::~DataSourceCheckBox()
{
}
