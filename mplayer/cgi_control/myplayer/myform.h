#ifndef MYFORM_H
#define MYFORM_H

#include <QWidget>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
class MyForm:public QWidget
{
	Q_OBJECT
public:
	MyForm(QWidget *parent=0);

public slots:
	void do_pause();
	void do_readUnusedMsg();
	void do_readMsg();
        //套接字相关的槽函数
        void myAccept();
        void processMsg();

private:
	QWidget *render;//显示视频的子窗体
	QProcess *mp;
        QTcpServer *server;
        QTcpSocket *socket;
};

#endif

