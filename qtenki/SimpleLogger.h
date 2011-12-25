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
			SystemShort=1,
			SystemLong=2,
			ISO8601=3,
			SplitISO8601=4,
			ISO8601TimeOnly=5,
		};

		SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt, enum SimpleLogger::DecimalType dt, enum SimpleLogger::TimeStampFormat tfmt);
		~SimpleLogger();
		void addSource(QString src, QString alias);
		void setUseUTC(bool use);
	
	protected:
		void writeHeader();
		void run();
		void logItem(QString str, int last = 0);
		void logValue(float v, int last = 0);
		void logLineEnd();
		void colTitles();
		void tsTitlesPre(int step);

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

		enum TimeStampFormat timestamp_format;
		bool use_utc;

		int count;
};

