#include "SimpleLogger.h"
#include <QtGui>
//#include <QHostInfo>
#include "version.h"

SimpleLogger::SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt)
{
	this->output_file = output_file;
	this->interval_s = interval_s;
	this->fmt = fmt;

	tenkisources = ts;
	count = 0;
}

SimpleLogger::~SimpleLogger()
{
}

void SimpleLogger::addSource(QString src)
{
	sources.append(src);
}

void SimpleLogger::writeHeader()
{
	QDateTime creation_time = QDateTime::currentDateTime();
	//QString hostname = QHostInfo::localHostName();

	file->write("# qtenki version ");
	file->write(g_version);
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
	char tmpbuf[32];

	sprintf(tmpbuf, "%0.02f", v);
	
	logItem(QString::fromAscii(tmpbuf), last);

}

void SimpleLogger::logLineEnd()
{
	file->write("\n");
	file->flush();
	count++;
	emit logged(count);
}

void SimpleLogger::colTitles()
{
	// Source names: AAAAAA:00
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(sd->q_name, i==(sources.size()-1));
	}

	// alias name TODO
	
	// measurement type
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(sd->chipShortString, i==(sources.size()-1));
	}

	// unit
	for (int i=0; i<sources.size(); i++)
	{
		struct sourceDescription *sd;
		sd = tenkisources->getSourceByName(sources.at(i));
		logItem(QString::fromAscii(unitToString(sd->chn_data->converted_unit,1)), i==(sources.size()-1));
	}

}

void SimpleLogger::doLog()
{
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

