#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <QString>
#include <QFile>

class Logger
{
public:
	explicit Logger();
	~Logger();

public:
	void Log(const char* format, ...);

	void errorLog(QString errMsg);

private:
	QFile*	m_pLogFile;
};

extern Logger g_Logger;

#endif // __LOGGER_H__
