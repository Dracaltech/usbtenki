#include "SimpleLogger.h"
#include "../common/usbtenki_version.h"
#include <QtGui>
//#include <QHostInfo>

SimpleLogger::SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt, enum SimpleLogger::DecimalType dt, enum SimpleLogger::TimeStampFormat tfmt)
{
	this->output_file = output_file;
	this->interval_s = interval_s;
	this->fmt = fmt;

	tenkisources = ts;
	count = 0;

	switch(dt)
	{
		default:
		case SystemFormat:
			logLocale = new QLocale();
			break;

		case Comma:
			logLocale = new QLocale(QLocale::French, QLocale::Canada);
			break;

		case Period:
			logLocale = new QLocale(QLocale::C, QLocale::AnyCountry);
			break;
	}

	timestamp_format = tfmt;	
	use_utc = 0;
}

SimpleLogger::~SimpleLogger()
{
	delete logLocale;
}

void SimpleLogger::addSource(QString src, QString alias)
{
	sources.append(src);
	aliases.append(alias);
}

void SimpleLogger::writeHeader()
{
	QDateTime creation_time = QDateTime::currentDateTime();
	//QString hostname = QHostInfo::localHostName();

	file->write("# USBTenki version ");
	file->write(USBTENKI_VERSION);
	file->write(" log file\n");

	file->write("# original filename: ");
	file->write(output_file.toAscii());
	file->write("\n");

	file->write("# creation date: ");
//	file->write("on machine ");
//	file->write(hostname.toAscii());
	file->write(creation_time.toString("yyyy-MM-dd hh:mm:ss").toAscii());	
	file->write("\n");

	

	file->write("#\n");
}

void SimpleLogger::run()
{
	timer = new QTimer();
	timer->setInterval(interval_s * 1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(doLog()), Qt::DirectConnection);

	if (!sources.size()) {
		emit logMessage("No sources found. Stopping.");
		return;
	}

	emit logMessage("opening file..");

	file = new QFile(output_file);
	file->open(QIODevice::Text | QIODevice::WriteOnly /*| QIODevice::Append*/);

	writeHeader();

	colTitles();
	doLog();

	timer->start();
	exec();
	timer->stop();

	delete timer;

	file->flush();
	file->close();
	emit logMessage("file closed.");
}

void SimpleLogger::logItem(QString str, int last)
{
	file->write(str.toAscii());
	
	if (last) {
		logLineEnd();
		return;
	}

	switch (fmt)
	{
		case SimpleLogger::Csv:
			file->write(", ");
			break;
		case SimpleLogger::Tsv:
			file->write("\t ");
			break;
		case SimpleLogger::Ssv:
			file->write(" ");
			break;
		case SimpleLogger::Scsv:
			file->write("; ");
			break;
	}
}


void SimpleLogger::logValue(float v, int last)
{
	//char tmpbuf[32];

//	sprintf(tmpbuf, "%0.02f", v);
	
	// TODO : Potential options:
	//   - scientific notation
	//   - precision (now hardcoded to 2)
	logItem(logLocale->toString(v, 'f', 2), last); 	
//	logItem(QString::fromAscii(tmpbuf), last);

}

void SimpleLogger::logLineEnd()
{
	file->write("\n");
	file->flush();
	count++;
	emit logged(count);
}

void SimpleLogger::tsTitlesPre(int step)
{
	// 0: Source names
	// 1: Aliases
	// 2: Measurement type
	// 3: Unit	

	switch(timestamp_format)
	{
		case None:
			break;

		case SystemShort:
			logItem("Datetime");
			break;

		case SystemLong:
			logItem("Datetime");
			break;

		case ISO8601:
			logItem("Datetime");
			break;

		case SplitISO8601:
			logItem("Date");
			logItem("Time");
			break;

		case ISO8601TimeOnly:
			logItem("Time");
			break;
	}


}

void SimpleLogger::colTitles()
{
	// Source names: AAAAAA:00
	tsTitlesPre(0);
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(sd->q_name, i==(sources.size()-1));
	}

	// aliases
	// this works with the assumption that sources and aliases QLists share
	// the same indices.
	tsTitlesPre(1);
	for (int i=0; i<aliases.size(); i++)
	{
		logItem(aliases.at(i), i==(aliases.size()-1));
	}

	
	// measurement type
	tsTitlesPre(2);
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(sd->chipShortString, i==(sources.size()-1));
	}

	// unit
	tsTitlesPre(3);
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(QString::fromAscii(unitToString(sd->chn_data->converted_unit,1)), i==(sources.size()-1));
	}

}

void SimpleLogger::setUseUTC(bool use)
{
	use_utc = use;
}

void SimpleLogger::doLog()
{
	// Requires QT 4.7
//	if (use_utc) {
//		QDateTime now = QDateTime::currentDateTimeUtc();
//	} else {
		QDateTime now = QDateTime::currentDateTime();
//	}

	switch(timestamp_format)
	{
		case None:
			break;

		case SystemShort:
			logItem(now.toString(Qt::SystemLocaleShortDate));
			break;

		case SystemLong:
			logItem(now.toString(Qt::SystemLocaleLongDate));
			break;

		case ISO8601:
			logItem(now.toString(Qt::ISODate));
			break;

		case SplitISO8601:
			logItem(now.toString("yyyy-mm-dd"));
			logItem(now.toString("hh:mm:ss"));
			break;

		case ISO8601TimeOnly:
			logItem(now.toString("hh:mm:ss"));
			break;
	}


	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;

		sd = tenkisources->getSourceByName(sources.at(i));

		if (sd == NULL) {
			emit logMessage("Source '" + sources.at(i)+ "' not found");
		} else {
			logValue(sd->chn_data->converted_data, i==(sources.size()-1));
			// write values to file
		}
	}
}

