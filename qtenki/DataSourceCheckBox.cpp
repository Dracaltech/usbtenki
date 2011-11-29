#include "DataSourceCheckBox.h"

DataSourceCheckBox::DataSourceCheckBox(QString caption, QString src)
{
	setText(caption);
	this->src = src;
}

DataSourceCheckBox::~DataSourceCheckBox()
{
}
