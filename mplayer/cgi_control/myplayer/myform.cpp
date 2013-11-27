#include "myform.h"

#include <QPalette>
#include <QPushButton>
#include <QProcess>
#include <QDebug>
#include <QTest>
#include <QHostAddress>

MyForm::MyForm(QWidget *parent)
:QWidget(parent)
{
        //创建Tcpserver对象，监听连接
        server = new QTcpServer(this);
        server->listen(QHostAddress::Any, 8800);
        connect(server, SIGNAL(newConnection()), this, SLOT(myAccept()));

	setGeometry(0, 0, 640, 480);
	render = new QWidget(this);
	render->setGeometry(100, 100, 300, 200);
	render->setAutoFillBackground(true);
	QPalette plt = render->palette();
	plt.setColor(QPalette::Background, Qt::blue);
	render->setPalette(plt);

	QPushButton *btn_pause = new QPushButton(this);
	btn_pause->setGeometry(450, 350, 100, 80);
	btn_pause->setText("&Pause");

	connect(btn_pause, SIGNAL(clicked()), this, SLOT(do_pause()));

	mp = new QProcess(this);
	connect(mp, SIGNAL(started()), this, SLOT(do_readUnusedMsg()));
	connect(mp, SIGNAL(readyReadStandardOutput()), 
			this, SLOT(do_readMsg()));
	QStringList args;
	//args << "/home/bryan/hd.mp4"; 
	args << "-playlist"
		<< "list.txt"
		<< "-x"
		<< QString::number(render->width())
		<< "-y"
		<< QString::number(render->height())
		<< "-zoom"
                << "-wid"
                << QString::number(render->winId())
		<< "-quiet"
		<< "-loop"
		<< "0"
		<< "-slave";

	mp->start("mplayer", args);


}

void MyForm::myAccept()
{
    qDebug() << "myAccept()" << endl;
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(processMsg()));
}

void MyForm::processMsg()
{
    qDebug() << "processMsg()" << endl;
    QByteArray msg;
    while(socket->bytesAvailable() > 0)
    {
        msg += socket->read(1024);
    }
    QString mymsg = msg;
    if(mymsg.compare("p")==0)
    {
        qDebug() << "send pause command!" << endl;
        mp->write("pause\n");
    }
    else if(mymsg.compare("full")==0)
    {
        mp->write("");
    }
    else if(mymsg.compare("quickmove")==0)
    {

    }
}

void MyForm::do_pause()
{
	QByteArray cmd;
        //cmd = "pause\n";
        cmd = "get_percent_pos\n";
        //cmd = "get_file_name\n";
        //cmd = "speed_set 10\n";
	mp->write(cmd);
	//cmd = "get_file_name\n";
	//cmd = "get_percent_pos\n";
	//mp->write(cmd);
	//qDebug() << "filename:" << mp->readAllStandardOutput();
}

void MyForm::do_readUnusedMsg()
{
	//等3秒之后再读
	QTest::qWait(3000);
	mp->readAllStandardOutput();
}

void MyForm::do_readMsg()
{
	QByteArray msg;
	msg = mp->readAllStandardOutput();
	qDebug() << "filename:" << msg;

}
