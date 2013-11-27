#include "mainframe.h"
#include <QPalette>
//#include <QPicture>

MainWindow::MainWindow(QWidget *parent)		//构造
:QWidget(parent)
{
	this->setWindowTitle(tr("广告机终端"));
	setGeometry(0, 0, 840, 480);	//默认大小,如果没有指定大小或全屏方式打开,则使用这个大小打开
	int width = this->width();
	int height = this->height();

	//主界面载入背景图片
	background = new QLabel(this);
	pic_background = new QPixmap(PICTURE_BACKGROUND);
	background->setPixmap(pic_background->scaled(this->width(), this->height()));

	//lcd时钟加载
	lcd = new Lcdtime(8, this);
	lcd_isload = true;					//lcd载入状态置为true

	//图片1加载
	pic1 = new Picture(this, 1);
	//pic1->setGeometry(width * 0.76, height * 0.26, width * 0.23, height * 0.29);	//原高:height * 0.495
	//pic1->showlogo();
	pic1->showpicture();
	pic1_isshow = pic1_isload = true;

	//图片2加载
	pic2 = new Picture(this, 2);
	pic2->showpicture();
	pic2_isshow = pic2_isload =  true;

	//图片3加载
	pic3 = new Picture(this, 3);
	pic3_isshow = pic3_isload =  true;

	//pic3->setGeometry(width * 0.76, height * 0.095, width * 0.23, height * 0.155);
	//textthread = new Qthread(this);

	//字幕加载
	text = new Textarea(this);
	QString msg("亚嵌教育:中图嵌入式技术的黄埔军校!");		//默认文字
	text->setmsg(msg);

	//网络监听
	server = new QTcpServer(this);
        server->listen(QHostAddress("0.0.0.0"), 8000);		//监听
	connect(server, SIGNAL(newConnection()), this, SLOT(slotnewConnect()));	//当有新连接时来时触发

	//视频区域与mplayer
	video = new QWidget(this);
	video->setGeometry(0, 0, width * 0.75, height * 0.85);
	char listname[] = LISTNAME;						//缺省播放列表,宏定义
	open_mplayer(listname, 1);						//mplayer程序
}

//打开mplayer播放机
void MainWindow::open_mplayer(char *filename, int flag)
{
	prc = new QProcess(this);		//创建进程
	QStringList args;

	if (flag == 1)					//载入列表模式打开
		args << "-playlist" << filename;
	else							//载入文件模式打开
		args << filename;
	args  << "-loop" << "0" << "-wid" << QString::number(video->winId())
		<< "-x" << QString::number(video->width()) << "-y" <<
		QString::number(video->height()) << "-zoom" << "-slave";
		//<< "-quiet";				//安静模式打开,控制命令不会显示在视频播放里

	//prc->setProcessChannelMode(QProcess::ForwardedChannels);
	prc->start("./mplayer", args);
	isplay = state = true;					//标记当前播放状态
	issoundenable = true;			//声音开启

	connect(prc, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(slotprocess_state(QProcess::ProcessState)));
	connect(prc, SIGNAL(finished(int)), this, SLOT(slotprocess_finish(int)));

	qDebug() << "Load mplayer over";
}

void MainWindow::slotprocess_finish(int num)			//进程结束时触发
{
	qDebug() << "process_finished, num=" << num;
	state = false;
}

void MainWindow::slotprocess_state(QProcess::ProcessState st)		//进程状态改变时触发
{
	qDebug() << "Process changed";
	if (st == 0) {				//是否是关闭状态0:QProcess::NotRunning
		qDebug() << "State changed closed";
		prc = NULL;
	}
}

void MainWindow::resizeEvent(QResizeEvent *ev)			//大小改变时,各模块随之重新设计大小,实现随意缩放
{
	int width = this->width();
	int height = this->height();

	background->setGeometry(0, 0, width, height);
	background->setPixmap(pic_background->scaled(width, height));
	video->setGeometry(0, 0, width * 0.75, height * 0.85);
	lcd->setGeometry(width * 0.76, height * 0.01, width * 0.23, height * 0.075);
	pic1->setGeometry(width * 0.76, height * 0.26, width * 0.23, height * 0.29);
	pic2->setGeometry(width * 0.76, height * 0.56, width * 0.23, height * 0.29);
	pic3->setGeometry(width * 0.76, height * 0.095, width * 0.23, height * 0.155);
	text->setGeometry(0, height * 0.86, width, height * 0.14);
}


void MainWindow::slotnewConnect()		//有新连接时触发
{
	qDebug() << "new connect";
	socket = server->nextPendingConnection();
	connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataArrive()));
}

void MainWindow::read_status_msg()		//从player中读取当前视频状态信息
{
	QByteArray message;

	message = prc->readAllStandardOutput();
	buf += message;
	if (buf.contains("ANS_VIDEO_RESOLUTION"))	//已读到所有状态信息
		send_status_msg();				//处理并发送
}

void MainWindow::send_status_msg()		//发回视频状态信息到客户端(web服务器cgi程序)
{
	//qDebug() << "status=" << buf;
	socket->write(buf);
	disconnect(prc, SIGNAL(readyReadStandardOutput()), this, SLOT(read_status_msg()));
	buf.clear();						//清空buf数据
	socket->close();
}
