#include <QtGui>


#include "MainWindow.h"


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	MainWindow *window = new MainWindow();

	app.setStyle(new QCleanlooksStyle());
	
	QFile file(":raphnet.qss");
	file.open(QFile::ReadOnly);
	QString stylesheet = QLatin1String(file.readAll());
	app.setStyleSheet(stylesheet);

//	app.setStyleSheet("QGroupBox { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF); border-radius: 5px; border: 2px solid grey;}");

	window->show();

	return app.exec();
}
