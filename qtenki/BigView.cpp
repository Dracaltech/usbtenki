#include "BigView.h"
#include "BigLabel.h"
#include "globals.h"

BigView::BigView()
{
	lay = new QVBoxLayout();
	this->setLayout(lay);
	
	g_tenkisources->addSourcesTo(this);
}

BigView::~BigView(void)
{
}

void BigView::addTenkiSource(struct sourceDescription *sd)
{
	addSourceByName(sd->q_name);
}

void BigView::removeTenkiSource(struct sourceDescription *sd)
{
}

void BigView::addSourceByName(QString sname)
{
	// testing
	BigLabel *lbl = new BigLabel("...", sname);
	lay->addWidget(lbl);	
	labels.append(lbl);

}

void BigView::refreshView()
{
	QSettings settings;

	for (int i=0; i<labels.size(); i++) {
		labels.at(i)->refresh();
		if (settings.value("bigviewChecked/"+labels.at(i)->src_name).toBool()) {
			labels.at(i)->setVisible(true);
		} else {
			labels.at(i)->setVisible(false);
		}
	}
}
