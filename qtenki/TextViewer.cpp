#include "TextViewer.h"
#include <QFile>
#include <QString>

TextViewer::TextViewer(QString filename)
{
	editor = new QPlainTextEdit();
	editor->setReadOnly(true);
	editor->setLineWrapMode(QPlainTextEdit::NoWrap);

	btnbox = new QDialogButtonBox();
	closeBtn = new QPushButton(tr("Close"));
	btnbox->addButton(closeBtn, QDialogButtonBox::AcceptRole);
	
	QObject::connect(btnbox, SIGNAL(accepted()), this, SLOT(accept()));

	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		editor->appendPlainText("Error: Could not open file");
	}
	else {
		while (!file->atEnd()) {
			QByteArray line = file->readLine();
			editor->appendPlainText(line.trimmed());
		}
	
		file->close();
	}

	editor->moveCursor(QTextCursor::Start);
	editor->setMinimumWidth(600);
	editor->setMinimumHeight(500);

	layout = new QVBoxLayout();
	setLayout(layout);

	layout->addWidget(editor);
	layout->addWidget(btnbox);
}

TextViewer::~TextViewer()
{

}

