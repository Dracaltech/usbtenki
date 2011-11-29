#include <QString>
#include <QThread>
#include <QList>
#include <QTimer>

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

		SimpleLogger(QString output_file, int interval_s, enum SimpleLogger::FileFormat fmt);
		~SimpleLogger();
		void addSource(QString src);

	protected:
		void run();

	public slots:
		void doLog();

	private:
		QString output_file;
		int interval_s;
		FileFormat fmt;
		QList<QString> sources;
		QTimer *timer;
};

