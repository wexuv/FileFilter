#include "sqliteDB.h"
#include "sqliteUtility/sqlite_result.h"
#include <QDir>

SQLite_DB g_SQliteDB;

SQLite_DB::SQLite_DB()
{
}
SQLite_DB::~SQLite_DB()
{

}

bool SQLite_DB::InitDB()
{
	QString dbPath = "./data/";
	QDir dbdir;
	dbdir.mkpath(dbPath);

	QString dbFile = dbPath + "./data.db";
	QFileInfo fi(dbFile);
	if (!fi.isFile())
	{
		if (!CreateDataBase(dbFile.toStdString().c_str()))
			return false;
	}

	if (!OpenDataBase(dbFile.toStdString().c_str()))
		return false;

	return true;
}

bool SQLite_DB::CreateDataBase(const char* szDBFile)
{
	//初始化DB文件
	if (driver::success != m_SqliteDB.open(szDBFile))
		return false;

	//设置密码
	//result = sqlite3_key(m_SqliteDB, CRYPT_KEY, sizeof(CRYPT_KEY));
	//清空密码
	//result=sqlite3_rekey(db,NULL,0); 

	//创建文件记录表
	m_SqliteDB.execute("CREATE TABLE [ff_files] ([md5] CHAR(32) PRIMARY KEY NOT NULL,[filesize] INTEGER not NULL,[filename] CHAR(256) not NULL,[filepath] CHAR(4096)  )");

	m_SqliteDB.close();

	return true;
}
bool SQLite_DB::OpenDataBase(const char* szDBFile)
{
	if (driver::success != m_SqliteDB.open(szDBFile))
		return false;

	//result = sqlite3_key(m_SqliteDB, CRYPT_KEY, sizeof(CRYPT_KEY));

	//result = sqlite3_exec(m_SqliteDB, "PRAGMA synchronous = OFF", 0, 0, 0);    //提高性能
	//result = sqlite3_exec(m_SqliteDB, "PRAGMA cache_size = 8000", 0, 0, 0); //加大缓存
	//result = sqlite3_exec(m_SqliteDB, "PRAGMA count_changes = 1", 0, 0, 0); //返回改变记录数
	//result = sqlite3_exec(m_SqliteDB, "PRAGMA case_sensitive_like = 1", 0, 0, 0); //支持中文LIKE查询

	return true;
}
void SQLite_DB::CloseDataBase()
{
	m_SqliteDB.close();
}

void SQLite_DB::ClearDataBase()
{
	m_SqliteDB.execute("drop TABLE [ff_files]");

	m_SqliteDB.close();

	QFile::remove("./data/data.db");
}

bool SQLite_DB::InsertFile(QString md5, QString filename, QString filepath, int filesize)
{
	if (md5 == NULL || filename == NULL || filepath == NULL)
		return false;

	TransString(md5);
	TransString(filename);
	TransString(filepath);
	//if (!StringCheck(md5, (int)strlen(md5)) || !StringCheck(filename, (int)strlen(filename)) || !StringCheck(filepath, (int)strlen(filepath)))
	//	return false;

	char sql[256] = { 0 };
	sprintf_s(sql, "INSERT INTO ff_files(md5, filesize, filename,filepath) VALUES('%s',%d,'%s', '%s')", md5.toStdString().c_str(), filesize, filename.toStdString().c_str(), filepath.toStdString().c_str());

	int32_t result = m_SqliteDB.execute(sql);

	return result == driver::success;
}
bool SQLite_DB::SelectFile(QString md5, QString& filename, QString& filepath, int& filesize)
{
	if (md5 == NULL)
		return false;

	TransString(md5);
	//if (!StringCheck(md5, (int)strlen(md5)))
	//	return false;

	char sql[256] = { 0 };
	sprintf_s(sql, "select md5,filesize,filename,filepath from ff_files where md5='%s'", md5.toStdString().c_str());

	driver::SQLite_Result kResult;
	if (driver::success != m_SqliteDB.execute(sql, kResult))
	{
		//没有数据或者执行失败
		return false;
	}

	if (kResult.fetch_next_row())
	{
		char tmpmd5[32] = { 0 };
		kResult.get_string(tmpmd5, 32);

		kResult >> filesize;

		char tmpfilename[256] = { 0 };
		kResult.get_string(tmpfilename, 256);
		filename = tmpfilename;

		char tmpfilepath[4096] = { 0 };
		kResult.get_string(tmpfilepath, 4096);
		filepath = tmpfilepath;

		return true;
	}

	return false;
}
void SQLite_DB::TransString(QString& src)
{
	src = src.replace("/", "//");
	src = src.replace("'", "''");
	src = src.replace("[", "/[");
	src = src.replace("]", "/]");
	src = src.replace("%", "/%");
	src = src.replace("&", "/&");
	src = src.replace("_", "/_");
	src = src.replace("(", "/(");
	src = src.replace(")", "/)");
}

bool SQLite_DB::StringCheck(const char* szSrc, int nSrcLen)
{
	if (nSrcLen <= 0 || szSrc == NULL)
		return false;
	for (int i = 0; i < nSrcLen; ++i)
	{
		switch (szSrc[i])
		{
		case '\0':
			return true;
		case '\'':
		case '\"':
		case '(':
		case ')':
		case '=':
		case '\\':
			return false;
		}
	}
	return true;
}
