#include "log.h"
#include <QTextStream>
#include <QDebug>

Logger g_Logger;

Logger::Logger()
{
}

Logger::~Logger()
{
	if (m_pLogFile != NULL)
	{
		m_pLogFile->close();
		delete m_pLogFile;
	}
}

void Logger::Log(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	char message[800];
	_vsnprintf(message, 800, format, arglist);
	va_end(arglist);

	errorLog(message);
}

void Logger::errorLog(QString errMsg)
{
	if (m_pLogFile == NULL)
	{
		m_pLogFile = new QFile(".\\data\\error.log");
		m_pLogFile->open(QIODevice::Append | QIODevice::Text);
	}

	QTextStream in(m_pLogFile);
	in << errMsg << "\n";

	qDebug() << errMsg;
}