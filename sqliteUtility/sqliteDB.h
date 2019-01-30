#ifndef __SQLITE_DB_H__
#define __SQLITE_DB_H__

#include <string>
#include <stdint.h>
#include "sqliteUtility/sqlite_handler.h"
#include <QString>

class SQLite_DB
{
public:
	SQLite_DB(void);
	~SQLite_DB(void);

public:
	bool InitDB();
	bool CreateDataBase(const char* szDBFile);
	bool OpenDataBase(const char* szDBFile);
	void CloseDataBase();
	void ClearDataBase();

	bool InsertFile(QString md5, QString filename, QString filepath, int filesize);
	bool SelectFile(QString md5, QString& filename, QString& filepath, int& filesize);
	void TransString(QString& src);
	bool StringCheck(const char* szSrc, int nSrcLen);

private:
	driver::SQLite_Handler m_SqliteDB;
};

extern SQLite_DB g_SQliteDB;
#endif	// __SQLITE_DB_H__


