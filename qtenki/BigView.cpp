#include "BigView.h"
#include "BigLabel.h"

BigView::BigView()
{
	lay = new QVBoxLayout();
	this->setLayout(lay);
	
	addSourceByName("SE95_0:00");
	addSourceByName("B10004:00");		
	addSourceByName("B10004:01");		
	addSourceByName("B10004:02");		
}

BigView::~BigView(void)
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
	for (int i=0; i<labels.size(); i++) {
		labels.at(i)->refresh();
	}
}

