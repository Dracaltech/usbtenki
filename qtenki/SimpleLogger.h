#include <QString>
#include <QThread>
#include <QList>
#include <QTimer>
#include <QFile>
#include <QLocale>

#include "TenkiSources.h"

class SimpleLogger : public QThread
{
	Q_OBJECT

	public:
		enum FileFormat {
			Csv=0, // comma separated
			Tsv=1, // tab separated
			Ssv=2, // space separated
			Scsv=3 // semicolon separated
		};
		enum DecimalType {
			SystemFormat=0,
			Period=1,
			Comma=2,
		};
		enum TimeStampFormat {
			None=0,
			ISO8601=1,
			SplitISO8601=2,
			SystemShort=3,
			SystemLong=4,
		};

		SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt, enum SimpleLogger::DecimalType dt, enum SimpleLogger::TimeStampFormat tfmt);
		~SimpleLogger();
		void addSource(QString src, QString alias);
	
	protected:
		void writeHeader();
		void run();
		void logItem(QString str, int last = 0);
		void logValue(float v, int last = 0);
		void logLineEnd();
		void colTitles();

	public slots:
		void doLog();

	signals:
		void logMessage(QString msg);
		void logged(int counter);

	private:
		TenkiSources *tenkisources;

		QString output_file;
		int interval_s;
		FileFormat fmt;
		QList<QString> sources;
		QList<QString> aliases; 
		QTimer *timer;
		QFile *file;

		QLocale *logLocale;

		int count;
};

