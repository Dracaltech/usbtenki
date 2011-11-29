#include "SimpleLogger.h"
#include <QtGui>

SimpleLogger::SimpleLogger(QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt)
{
	this->output_file = output_file;
	this->interval_s = interval_s;
	this->fmt = fmt;

	timer = new QTimer();
	timer->setInterval(interval_s * 1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(doLog()));
}

SimpleLogger::~SimpleLogger()
{
	delete timer;
}

void SimpleLogger::addSource(QString src)
{
	sources.append(src);
}

void SimpleLogger::run()
{
	timer->start();
	exec();
}

void SimpleLogger::doLog()
{
	qDebug() << "Would log now";
}

