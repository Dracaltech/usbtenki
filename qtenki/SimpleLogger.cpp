#include "SimpleLogger.h"
#include <QtGui>

SimpleLogger::SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt)
{
	this->output_file = output_file;
	this->interval_s = interval_s;
	this->fmt = fmt;

	tenkisources = ts;


}

SimpleLogger::~SimpleLogger()
{
}

void SimpleLogger::addSource(QString src)
{
	sources.append(src);
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
	file->open(QIODevice::WriteOnly | QIODevice::Append);

	doLog();

	timer->start();
	exec();
	timer->stop();

	delete timer;

	file->flush();
	file->close();
	emit logMessage("file closed.");
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
			// write values to file
		}
	}
}

