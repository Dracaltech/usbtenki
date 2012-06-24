#ifndef _bigview_h__
#define _bigview_h__

#include <QtGui>
#include <QWidget>

#include "BigLabel.h"

class BigView : public QWidget
{
	Q_OBJECT

	public:
		BigView();
		~BigView(void);

	public slots:
		void refreshView(void);
		void addSourceByName(QString src_name);
	
	private:
		QPushButton *addBtn;
		QVBoxLayout *lay;
		QList<BigLabel*> labels;
};

#endif 

