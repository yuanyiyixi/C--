#include "mainframe.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	MainWindow mainwindow;
	if (!argv[1])
		mainwindow.show();					//默认大小

	else if (!strcmp(argv[1], "FULL"))
		mainwindow.showFullScreen();		//全屏方式

	else {									//指定大小打开
		mainwindow.setGeometry(0, 0, strtol(argv[1], NULL, 10), strtol(argv[2], NULL, 10));
		mainwindow.show();
	}

	return app.exec();
}
