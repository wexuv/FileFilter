#include "FilterThread.h"
#include <QString>
#include <QDateTime>
#include <QDir>
#include "sqliteUtility/sqliteDB.h"
#include <QCryptographicHash>

QFilterThread::QFilterThread()
{
	QObject::connect(&m_kFilterRunThread, &QThread::started, this, &QFilterThread::run);
	QObject::connect(&m_kFilterRunThread, &QThread::finished, this, &QFilterThread::threadQuit);

	moveToThread(&m_kFilterRunThread);

	m_bStopImmediatly = false;
}

QFilterThread::~QFilterThread()
{
	Close();
}

bool QFilterThread::Open(QString filePath)
{
	m_FilterPath = filePath;

	m_kFilterRunThread.start();

	return true;
}

void QFilterThread::Close()
{
	m_kFilterRunThread.quit();
	m_kFilterRunThread.wait();
}

bool QFilterThread::IsRunning()
{
	return m_kFilterRunThread.isRunning();
}

void QFilterThread::Stop()
{
	m_bStopImmediatly = true;
}

void QFilterThread::run()
{
	try
	{
		QDateTime	m_kLastTime = QDateTime::currentDateTime();

		Traverse(m_FilterPath);

		QDateTime curTime = QDateTime::currentDateTime();

		qint64  elapse = curTime.toMSecsSinceEpoch() - m_kLastTime.toMSecsSinceEpoch();
	}
	catch (...)
	{
		int a = 0;
	}

	Close();
}

void QFilterThread::threadQuit()
{
	emit sigCurrentFile(m_FilterPath + " done!\r\n");
}


void QFilterThread::Traverse(QString path)
{
	if (m_bStopImmediatly)
		return;

	QDir dir(path);
	if (!dir.exists())
	{
		return;
	}
	dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();

	int file_count = list.count();
	if (file_count <= 0)
	{
		return;
	}

	QStringList string_list;
	for (int i = 0; i < file_count; ++i)
	{
		QFileInfo file_info = list.at(i);
		if (file_info.isDir())
		{
			Traverse(file_info.absoluteFilePath());
			continue;
		}
		QString suffix = file_info.suffix();
		//if (QString::compare(suffix, QString("png"),Qt::CaseInsensitive) == 0)
		//{
		//	QString absolute_file_path = file_info.absoluteFilePath();
		//		string_list.append(absolute_file_path);
		//}

		emit sigCurrentFile(file_info.fileName());

		QFile theFile(file_info.absoluteFilePath());
		theFile.open(QIODevice::ReadOnly);
		QByteArray ba;
		if (file_info.size() < 1000)
		{
			ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
		}
		else
		{
			QByteArray fileBinary = theFile.read(1024);
			ba = QCryptographicHash::hash(fileBinary, QCryptographicHash::Md5);
		}

		theFile.close();

		bool bInsertResult = g_SQliteDB.InsertFile(ba.toHex().constData(), file_info.fileName().toStdString().c_str(), file_info.absoluteFilePath().toStdString().c_str(), file_info.size());

		if (!bInsertResult)
		{
			QString duplFileName, duplFilePath;
			int duplFileSize = 0;
			bool bSelectResult = g_SQliteDB.SelectFile(ba.toHex().constData(), duplFileName, duplFilePath, duplFileSize);

			if (!bSelectResult)
			{
				emit sigFileInfo(0, file_info.fileName(), file_info.absoluteFilePath(), ba.toHex().constData(), file_info.size());
			}
			else
			{
				if (file_info.size() == duplFileSize)
				{
					emit sigFileInfo(1, file_info.fileName(), file_info.absoluteFilePath(), ba.toHex().constData(), file_info.size());
					emit sigFileInfo(2, duplFileName, duplFilePath, ba.toHex().constData(), duplFileSize);
				}
			}
		}
	}
}