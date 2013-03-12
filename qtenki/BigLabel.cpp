#include <iostream>
#include "BigLabel.h"
#include "globals.h"

#include <QFont>
#include <QLabel>
#include <QWidget>

BigLabel::BigLabel(const QString &text, QString source_name)
{
	setText(text);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	src_name = source_name;
	refresh();
}

void BigLabel::refresh()
{
	QSettings settings;
	struct USBTenki_channel chndata;

	struct sourceDescription *sd = g_tenkisources->getSourceByName(src_name);
	if (!sd) {
		setText("err");
		return;
	}

	g_tenkisources->convertToUnits(sd->chn_data, &chndata);

	QString alias = sd->q_alias;
	QString units = QString::fromUtf8(unitToString(chndata.converted_unit, 0));

	QString d;

	g_tenkisources->formatValue(&d, chndata.converted_data);

	QString final;

	if (settings.value("bigview/show_aliases").toBool()) {
		final += alias;
		final += ": ";
	}

	final += d;
	
	if (settings.value("bigview/show_units").toBool()) {
		final += " ";
		final += units;
	}

	setText(final);
}

void BigLabel::resizeEvent(QResizeEvent *event)
{
	int flags = Qt::TextDontClip; 
	QRect resize(0,0,event->size().width(), event->size().height());
	QFont f = font();

	
	// Step 1: Try to set the requested height. If everything fits horizontally, 
	// all labels will have equal height.
	f = font();
	float orig = event->size().height() * 0.8;
	f.setPixelSize(orig);
	
	
	// Step 2: Long strings may be cut. If this happens,
	// reduce the font until it fits in the target width.
	QRect fontBoundRect = QFontMetrics(f).boundingRect(resize,flags, text());

//	qDebug() << "font width: " << fontBoundRect.width() << " space:" << resize.width() << " Text: " << text();

	while (fontBoundRect.width() > resize.width() && orig > 0 ) {

		f.setPixelSize(orig);
		
		orig -= (orig*0.1); // 10% decrements. Every resize then requires approx. 10 iterations
		
//		qDebug() << "Testing size: " << orig;
	
		fontBoundRect = QFontMetrics(f).boundingRect(resize,flags, text());
	}

	setFont(f);
	
	QFrame::resizeEvent(event);
}

