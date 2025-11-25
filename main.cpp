#include "ui/MainWindow.h"
#include "inc/appinit.h"
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QtGlobal>
#include <QSurfaceFormat>

void initHighDpi()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

/*
UI工程使用的是这里的：
https://blog.csdn.net/hwc3737/article/details/108377607
*/
int main(int argc, char* argv[])
{
	initHighDpi();

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":logo.ico"));
	//加载样式表
	QFile file(":/css/index.css");
	if (file.open(QFile::ReadOnly))
	{
		QString qss = QLatin1String(file.readAll());
		qApp->setStyleSheet(qss);
		file.close();
	}

	//全局字体
#ifdef Q_OS_WIN
	QFont font("Microsoft YaHei", 10);
	font.setStyleStrategy(QFont::NoSubpixelAntialias);
	a.setFont(font);
#elif defined(Q_OS_LINUX)
	QFont font("Noto Sans SC", 10);
	font.setStyleStrategy(QFont::NoSubpixelAntialias);
	a.setFont(font);
#elif defined(Q_OS_MAC)
	
#endif
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);


	//QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	//屏幕拖动
	AppInit::Instance()->start();

	MainWindow w;
	w.show();

	return a.exec();
}
