#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FilterThread.h"
#include <QTimer>
#include <list>

namespace Ui {
class MainWindow;
}

struct FilterFileInfo
{
	QString fileName;
	QString filePath;
	QString fileMD5;
	int filesize;
	int status;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
	virtual void closeEvent(QCloseEvent*event);

private slots:
	void OnSelFolder();
	void OnSigCurrentFile(QString curFile);
	void OnSigFileInfo(int status, QString filename, QString fullfilePath, QString md5, int filesize);

	void checkCacheLst();
public:
	void InsertTableLine(QString md5, QString filename, QString filepath, int filesize, QColor color);

private:
    Ui::MainWindow *ui;
	int	m_nTraverseIndex;

	QFilterThread	m_kFileFilterThread;
	QTimer			m_kTimer;
	std::list<FilterFileInfo>	m_cacheLst;
};

#endif // MAINWINDOW_H
