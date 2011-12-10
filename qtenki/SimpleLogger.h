#include <QString>
#include <QThread>
#include <QList>
#include <QTimer>
#include <QFile>

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

		SimpleLogger(TenkiSources *ts, QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt);
		~SimpleLogger();
		void addSource(QString src);
	
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

	private:
		TenkiSources *tenkisources;

		QString output_file;
		int interval_s;
		FileFormat fmt;
		QList<QString> sources;
		QTimer *timer;
		QFile *file;

		bool first_log;
};

