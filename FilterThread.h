#ifndef _Q_FILTER_THREAD_H_
#define _Q_FILTER_THREAD_H_

#include <QObject>
#include <QThread>

class QFilterThread : public QObject
{
	Q_OBJECT

public:
	QFilterThread();
	~QFilterThread();

public:
	bool Open(QString filePath);
	void Close();

	bool IsRunning();
	void Stop();

signals:
	void sigCurrentFile(QString fullfilePath);
	void sigFileInfo(int status,QString filename,QString fullfilePath,QString md5,int filesize);

public slots:
	void run();
	void threadQuit();

private:
	void Traverse(QString path);

private:
	QThread		m_kFilterRunThread;

	QString		m_FilterPath;

	bool		m_bStopImmediatly;
};

#endif // _Q_FILTER_THREAD_H_
