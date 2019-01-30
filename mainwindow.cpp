#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QHeaderView>
#include "log.h"
#include "sqliteUtility/sqliteDB.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	connect(ui->selFolder, SIGNAL(triggered(bool)), this, SLOT(OnSelFolder()));

	connect(&m_kFileFilterThread, SIGNAL(sigCurrentFile(QString)), this, SLOT(OnSigCurrentFile(QString)));
	connect(&m_kFileFilterThread, SIGNAL(sigFileInfo(int,QString,QString,QString,int)), this, SLOT(OnSigFileInfo(int, QString, QString, QString, int)));

	ui->tableWidget->setColumnCount(4);
	ui->tableWidget->setRowCount(1000);

	QStringList header;
	header << QString::fromLocal8Bit("文件名")  << QString::fromLocal8Bit("文件大小") << QString::fromLocal8Bit("MD5码") << QString::fromLocal8Bit("路径");
	ui->tableWidget->setHorizontalHeaderLabels(header);

	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

	m_nTraverseIndex = 0;

	g_SQliteDB.InitDB();

	connect(&m_kTimer, SIGNAL(timeout()), this, SLOT(checkCacheLst()));
	m_kTimer.start(1);
}

MainWindow::~MainWindow()
{
	m_kTimer.stop();

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	if (m_kFileFilterThread.IsRunning())
	{
		m_kFileFilterThread.Stop();
	}
}

void MainWindow::OnSelFolder()
{
	if (m_kFileFilterThread.IsRunning())
		return;

	QMessageBox::StandardButton button;
	button = QMessageBox::question(this, QString::fromLocal8Bit("FileFilter"), QString::fromLocal8Bit("是否清理数据库？"), QMessageBox::Yes | QMessageBox::No);
	if (button == QMessageBox::Yes)
	{
		g_SQliteDB.ClearDataBase();
		g_SQliteDB.InitDB();
	}

	QString file_path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择文件路径..."), "./");
	if (file_path.isEmpty())
	{
		return;
	}

	m_cacheLst.clear();
	ui->tableWidget->clear();
	m_nTraverseIndex = 0;

	m_kFileFilterThread.Open(file_path);
}

void MainWindow::OnSigCurrentFile(QString curFile)
{
	ui->statusBar->showMessage(curFile);
}

void MainWindow::OnSigFileInfo(int status, QString filename, QString fullfilePath, QString md5, int filesize)
{
	FilterFileInfo filter;
	filter.fileName = filename;
	filter.filePath = fullfilePath;
	filter.fileMD5 = md5;
	filter.filesize = filesize;
	filter.status = status;
	m_cacheLst.push_back(filter);
}

void MainWindow::checkCacheLst()
{
	if(!m_cacheLst.empty())
	{
		std::list<FilterFileInfo>::iterator iter = m_cacheLst.begin();

		int status = iter->status;
		QString filename = iter->fileName;
		QString fullfilePath = iter->filePath;
		QString md5 = iter->fileMD5;
		int filesize = iter->filesize;

		if (status == 0)
		{
			//发生错误 
			g_Logger.Log("select file error:%s,%s,%s,%d", md5.toStdString().c_str(), filename.toStdString().c_str(), fullfilePath.toStdString().c_str(), filesize);
			InsertTableLine(md5, filename, fullfilePath, filesize, QColor(255, 0, 0));
		}
		else if (status == 1)
		{
			//重复文件1
			g_Logger.Log("duplicate file1:%s,%s,%s,%d", md5.toStdString().c_str(), filename.toStdString().c_str(), fullfilePath.toStdString().c_str(), filesize);
			InsertTableLine(md5, filename, fullfilePath, filesize, QColor(255, 255, 255));
		}
		else if (status == 2)
		{
			//重复文件2
			g_Logger.Log("duplicate file2:%s,%s,%s,%d", md5.toStdString().c_str(), filename.toStdString().c_str(), fullfilePath.toStdString().c_str(), filesize);
			InsertTableLine(md5, filename, fullfilePath, filesize, QColor(200, 200, 200));
		}

		m_cacheLst.pop_front();
	}
}

void MainWindow::InsertTableLine(QString md5,QString filename, QString filepath, int filesize,QColor color)
{
	if (m_nTraverseIndex >= ui->tableWidget->rowCount())
	{
		g_Logger.errorLog("too many rows");
		return;
	}

	ui->tableWidget->setItem(m_nTraverseIndex, 0, new QTableWidgetItem(filename));
	ui->tableWidget->setItem(m_nTraverseIndex, 1, new QTableWidgetItem(QString("%1").arg(filesize)));
	ui->tableWidget->setItem(m_nTraverseIndex, 2, new QTableWidgetItem(md5));
	ui->tableWidget->setItem(m_nTraverseIndex, 3, new QTableWidgetItem(filepath));

	for (int col = 0; col < ui->tableWidget->columnCount(); col++)
	{
		QTableWidgetItem *item = ui->tableWidget->item(m_nTraverseIndex, col);
		item->setBackgroundColor(color);
	}

	++m_nTraverseIndex;
}
