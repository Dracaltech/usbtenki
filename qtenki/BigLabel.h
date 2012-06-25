#ifndef _biglabel_h__
#define _biglabel_h__

#include <QtGui>

class BigLabel : public QLabel
{
	Q_OBJECT

	public:
		BigLabel(const QString &text, QString source_name);

		void resizeEvent(QResizeEvent *event);
		void refresh();
		QString src_name;
};

#endif // _biglabel_h__


