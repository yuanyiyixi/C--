#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QLabel>
#include <QCalendarWidget>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QTcpServer>
#include <QTcpSocket>
#include "picture.h"
#include "textarea.h"
#include "lcdtime.h"

#define PICTURE_BACKGROUND "./background.jpg"		//背景图片路径文件名
#define LISTNAME "./list/playlist.list"			//默认载入的播放列表路径文件名

class MainWindow :public QWidget
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);

public slots:
	void slotnewConnect(void);		//新连接
	void slotDataArrive(void);		//读取数据
	void read_status_msg(void);		//从mplayer中读取状态信息
	void slotprocess_state(QProcess::ProcessState);
	void slotprocess_finish(int);

private:
	QWidget *video;		//视频区域
	Lcdtime *lcd;			//时钟区域
	QLabel *background;	//背景图片区, 与主窗口一样大
	QPixmap *pic_background;	//存储背景图片颜色
	Textarea *text;		//字幕区
	Picture *pic1;		//图片1,右中角
	Picture *pic2;		//图片2,右下角
	Picture *pic3;		//图片3,右上角, 宽的


	bool isplay;			//是否在播放
	bool state;			//视频停止或播放状态
	bool issoundenable;	//声音是否开启
	bool lcd_isload;		//lcd是否载入
	bool pic1_isshow;		//图片1是否显示
	bool pic1_isload;		//图片1是否载入
	bool pic2_isshow;		//图片2是否显示
	bool pic2_isload;		//图片2是否载入
	bool pic3_isshow;		//图片3是否显示
	bool pic3_isload;		//图片3是否载入
	QByteArray buf;		//接收与发送数据容器
	QProcess *prc;		//mplayer进程
	QTcpServer *server;
	QTcpSocket *socket;

	void open_mplayer(char *, int flag);		//打开mplayer
	void resizeEvent(QResizeEvent *);		//大小改变
	void send_status_msg(void);				//发送视频状态信息
};

#endif // MAINFRAME_H
