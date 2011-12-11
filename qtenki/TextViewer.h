#ifndef _TextViewer_h__
#define _TextViewer_h__

#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

class TextViewer : public QDialog
{
	Q_OBJECT

	public:
		TextViewer(QString filename);
		~TextViewer();
	
	private:
		QPlainTextEdit *editor;
		QVBoxLayout *layout;
		QDialogButtonBox *btnbox;
		QPushButton *closeBtn;
			
};

#endif // _TextViewer_h__


